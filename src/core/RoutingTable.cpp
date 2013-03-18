/*
 * $FU-Copyright$
 */

#include "RoutingTable.h"
#include "Environment.h"
#include <utility>

namespace ARA {
    typedef std::shared_ptr<Address> AddressPtr;

    RoutingTable::RoutingTable() {
        lastAccessTime = nullptr;
    }

    RoutingTable::~RoutingTable() {
        std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>::iterator iterator;
        for (iterator=table.begin(); iterator!=table.end(); iterator++) {
            std::pair<AddressPtr const, std::deque<RoutingTableEntry*>*> entryPair = *iterator;
            std::deque<RoutingTableEntry*>* entryList = entryPair.second;
            // delete all RoutingTableEntries in the List
            while(entryList->empty() == false) {
                RoutingTableEntry* entry = entryList->back();
                entryList->pop_back();
                delete entry;
            }
            delete entryList;
        }
        table.clear();

        if(hasTableBeenAccessedEarlier()) {
            delete lastAccessTime;
        }
    }

    void RoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue) {
        RoutingTableEntry *entry = new RoutingTableEntry(nextHop, interface, pheromoneValue);
        update(destination, entry);
    }


    void RoutingTable::update(AddressPtr destination, RoutingTableEntry *pEntry) {
        if(isDeliverable(destination) == false) {
            std::deque<RoutingTableEntry*>* entryList = new std::deque<RoutingTableEntry*>();
            entryList->push_back(pEntry);
            table[destination] = entryList;
        } else {
            // there is at least one registered route for this destination
            std::deque<RoutingTableEntry*>* entryList = table[destination];
            bool entryHasBeenUpdated = false;
            for (auto& entry: *entryList) {
                if(entry->getAddress()->equals(pEntry->getAddress()) && entry->getNetworkInterface()->equals(pEntry->getNetworkInterface())) {
                    updateExistingEntry(entry, pEntry);
                    //entry->setPheromoneValue(pheromoneValue);
                    entryHasBeenUpdated = true;
                }
            }

            if(entryHasBeenUpdated == false) {
                entryList->push_back(pEntry);
            }else{
                delete pEntry;
            }
        }
    }

    void RoutingTable::updateExistingEntry(RoutingTableEntry *oldEntry, RoutingTableEntry *newEntry){
        oldEntry->setPheromoneValue(newEntry->getPheromoneValue());
    }

    void RoutingTable::removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
        if(isDeliverable(destination)) {
            std::deque<RoutingTableEntry*>* entryList = table[destination];
            std::deque<RoutingTableEntry*>::iterator iterator = entryList->begin();
            while(iterator != entryList->end()) {
                RoutingTableEntry* entry = *iterator;
                if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                    entryList->erase(iterator);
                    delete entry;
                    return;
                }
                iterator++;
            }
        }
    }

    std::deque<RoutingTableEntry*>* RoutingTable::getPossibleNextHops(AddressPtr destination) {
        if(isDeliverable(destination)) {
            // FIXME this should return a copy of this list (to avoid that entries in this list are accidentally removed from or added)
            return table[destination];
        }
        else {
            return new std::deque<RoutingTableEntry*>();
        }
    }

    std::deque<RoutingTableEntry*>* RoutingTable::getPossibleNextHops(const Packet* packet) {
        triggerEvaporation();
        return getPossibleNextHops(packet->getDestination());
    }

    bool RoutingTable::isDeliverable(AddressPtr destination) {
        triggerEvaporation();
        return table.find(destination) != table.end();
    }

    bool RoutingTable::isDeliverable(const Packet* packet) {
        return isDeliverable(packet->getDestination());
    }

    float RoutingTable::getPheromoneValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface) {
        triggerEvaporation();

        if(isDeliverable(destination)) {
            std::deque<RoutingTableEntry*>* entryList = table[destination];
            for (auto& entry: *entryList) {
                if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                    return entry->getPheromoneValue();
                }
            }
        }

        return 0;
    }

    bool RoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
        if(isDeliverable(destination)){
            std::deque<RoutingTableEntry*>* entries = table[destination];
            for (auto& entry: *entries) {
                if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)){
                    return true;
                }
            }
        }
        return false;
    }

    void RoutingTable::triggerEvaporation() {
        Time* currentTime = Environment::getClock()->makeTime();
        currentTime->setToCurrentTime();

        if (hasTableBeenAccessedEarlier() == false) {
            lastAccessTime = currentTime;
        }
        else {
            long timeDifference = currentTime->getDifferenceInMilliSeconds(lastAccessTime);
            delete currentTime;

            if(evaporationPolicy->isEvaporationNecessary(timeDifference)) {
                lastAccessTime->setToCurrentTime();

                std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>::iterator i;
                for (i=table.begin(); i!=table.end(); i++) {
                    std::pair<AddressPtr const, std::deque<RoutingTableEntry*>*> entryPair = *i;
                    std::deque<RoutingTableEntry*>* entryList = entryPair.second;

                    for (auto& entry: *entryList) {
                        float newPheromoneValue = evaporationPolicy->evaporate(entry->getPheromoneValue(), timeDifference);
                        if(newPheromoneValue > 0) {
                            entry->setPheromoneValue(newPheromoneValue);
                        }
                        else {
                            //TODO delete entry from table
                            entry->setPheromoneValue(newPheromoneValue);
                        }
                    }
                }
            }
        }
    }

    bool RoutingTable::hasTableBeenAccessedEarlier() {
        return lastAccessTime != nullptr;
    }

    void RoutingTable::setEvaporationPolicy(EvaporationPolicy* policy) {
        this->evaporationPolicy = policy;
    }

    EvaporationPolicy *RoutingTable::getEvaporationPolicy() const{
        return this->evaporationPolicy;
    }

} /* namespace ARA */
