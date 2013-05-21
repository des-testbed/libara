/*
 * $FU-Copyright$
 */

#include "RoutingTable.h"
#include "Environment.h"
#include "Exception.h"

#include <utility>

ARA_NAMESPACE_BEGIN

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
    if(table.find(destination) == table.end()) {
        // this is a new entry
        std::deque<RoutingTableEntry*>* entryList = new std::deque<RoutingTableEntry*>();
        entryList->push_back(pEntry);
        table[destination] = entryList;
    } else {
        // there is at least one registered route for this destination
        RoutingTableEntryList* entryList = table[destination];
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
    if(table.find(destination) != table.end()) {
        std::deque<RoutingTableEntry*>* entryList = table[destination];
        std::deque<RoutingTableEntry*>::iterator iterator = entryList->begin();

        if(iterator != entryList->end()) {
            while(iterator != entryList->end()) {
                RoutingTableEntry* entry = *iterator;
                if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                    entryList->erase(iterator);
                    delete entry;
                    break;
                }
                iterator++;
            }

            if(entryList->empty()) {
                // this was the last entry so we can delete the whole list
                table.erase(destination);
                delete entryList;
            }
        }
    }
}

std::deque<RoutingTableEntry*> RoutingTable::getPossibleNextHops(const Packet* packet) {
    if(isDeliverable(packet)) {
        return *(table[packet->getDestination()]);
    }
    else {
        // return empty list
        return std::deque<RoutingTableEntry*>();
    }
}

bool RoutingTable::isDeliverable(AddressPtr destination) {
    return table.find(destination) != table.end();
}

bool RoutingTable::isDeliverable(const Packet* packet) {
    AddressPtr destination = packet->getDestination();
    if(isDeliverable(destination)) {
        std::deque<RoutingTableEntry*>* entries = table[destination];
        if(entries->size() > 1) {
            // more than one route
            return true;
        }
        else {
            // check if the only available route leads to where we've got the packets from
            AddressPtr availableAddress = entries->front()->getAddress();
            return availableAddress->equals(packet->getSender()) == false;
        }
    }
    else {
        return false;
    }
}

float RoutingTable::getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    if(isDeliverable(destination)) {
        RoutingTableEntryList* entryList = table[destination];
        for (auto& entry: *entryList) {
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                return entry->getPheromoneValue();
            }
        }
    }

    return 0;
}

bool RoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
    if(isDeliverable(destination)) {
        std::deque<RoutingTableEntry*>* entries = table[destination];
        for (auto& entry: *entries) {
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)){
                return true;
            }
        }
    }

    return false;
}

bool RoutingTable::isNewRoute(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
    return exists(destination, nextHop, interface) == false;
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

            std::unordered_map<AddressPtr, RoutingTableEntryList*, AddressHash, AddressPredicate>::iterator i = table.begin();
            while(i!=table.end()) {
                std::pair<AddressPtr const, RoutingTableEntryList*> entryPair = *i;
                AddressPtr destination = entryPair.first;
                RoutingTableEntryList* entryList = entryPair.second;

                RoutingTableEntryList::iterator j = entryList->begin();
                while(j != entryList->end()) {
                    RoutingTableEntry* entry = *j;
                    float newPheromoneValue = evaporationPolicy->evaporate(entry->getPheromoneValue(), timeDifference);
                    if(newPheromoneValue > 0) {
                        entry->setPheromoneValue(newPheromoneValue);
                        j++;
                    }
                    else {
                        delete entry;
                        j = entryList->erase(j);
                    }
                }

                if(entryList->empty()) {
                    delete entryList;
                    i = table.erase(i);
                }
                else {
                    i++;
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

EvaporationPolicy* RoutingTable::getEvaporationPolicy() const{
    return this->evaporationPolicy;
}

unsigned int RoutingTable::getTotalNumberOfEntries() const {
    unsigned int tableSize = 0;
    std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>::const_iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        RoutingTableEntryList* entryList = iterator->second;
        tableSize += entryList->size();
    }
    return tableSize;
}

RoutingTableEntryTupel RoutingTable::getEntryAt(int wantedPosition) const {
    int currentPosition = 0;
    std::unordered_map<AddressPtr, RoutingTableEntryList*, AddressHash, AddressPredicate>::const_iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        AddressPtr destination = iterator->first;
        RoutingTableEntryList* entryList = iterator->second;
        for (auto& entry: *entryList) {
            if(currentPosition == wantedPosition) {
                RoutingTableEntryTupel tupel;
                tupel.destination = destination;
                tupel.entry = entry;
                return tupel;
            }
            else {
                currentPosition++;
            }
        }
    }

    throw Exception("RoutingTable::getEntryAt: Index out of bounds");
}

ARA_NAMESPACE_END
