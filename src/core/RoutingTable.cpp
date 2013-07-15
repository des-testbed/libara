/*
 * $FU-Copyright$
 */

#include "RoutingTable.h"
#include "Environment.h"
#include "Exception.h"

ARA_NAMESPACE_BEGIN

RoutingTable::RoutingTable() {
    lastAccessTime = nullptr;
}

RoutingTable::~RoutingTable() {
    RoutingTableMap::iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        RoutingTableEntryList* entryList = iterator->second;

        // delete all RoutingTableEntries in the List
        while (entryList->empty() == false) {
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
    RoutingTableEntry* entry = new RoutingTableEntry(nextHop, interface, pheromoneValue);
    update(destination, entry);
}


void RoutingTable::update(AddressPtr destination, RoutingTableEntry* newEntry) {
    if (table.find(destination) == table.end()) {
        // this is a new entry
        RoutingTableEntryList* entryList = new RoutingTableEntryList();
        entryList->push_back(newEntry);
        table[destination] = entryList;
    }
    else {
        // there is at least one registered route for this destination
        RoutingTableEntryList* entryList = table[destination];
        bool entryHasBeenUpdated = false;
        for (auto& entry: *entryList) {
            if (entry->getAddress()->equals(newEntry->getAddress()) && entry->getNetworkInterface()->equals(newEntry->getNetworkInterface())) {
                updateExistingEntry(entry, newEntry);
                entryHasBeenUpdated = true;
            }
        }

        if (entryHasBeenUpdated == false) {
            entryList->push_back(newEntry);
        }
        else{
            delete newEntry;
        }
    }
}

void RoutingTable::updateExistingEntry(RoutingTableEntry* oldEntry, RoutingTableEntry* newEntry){
    oldEntry->setPheromoneValue(newEntry->getPheromoneValue());
}

void RoutingTable::removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    if (table.find(destination) != table.end()) {
        RoutingTableEntryList* entryList = table[destination];
        RoutingTableEntryList::iterator iterator = entryList->begin();

        if (iterator != entryList->end()) {
            while (iterator != entryList->end()) {
                RoutingTableEntry* entry = *iterator;
                if (entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                    entryList->erase(iterator);
                    delete entry;
                    break;
                }
                iterator++;
            }

            if (entryList->empty()) {
                // this was the last entry so we can delete the whole list
                table.erase(destination);
                delete entryList;
            }
        }
    }
}

RoutingTableEntryList RoutingTable::getPossibleNextHops(const Packet* packet) {
    if (isDeliverable(packet)) {
        return *(table[packet->getDestination()]);
    }
    else {
        // return empty list
        return RoutingTableEntryList();
    }
}

RoutingTableEntryList RoutingTable::getPossibleNextHops(AddressPtr destination) {
    if (isDeliverable(destination)) {
        return *(table[destination]);
    }
    else {
        // return empty list
        return RoutingTableEntryList();
    }
}

bool RoutingTable::isDeliverable(AddressPtr destination) {
    return table.find(destination) != table.end();
}

bool RoutingTable::isDeliverable(const Packet* packet) {
    AddressPtr destination = packet->getDestination();
    if (isDeliverable(destination)) {
        RoutingTableEntryList* entries = table[destination];
        if (entries->size() > 1) {
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
    if (isDeliverable(destination)) {
        RoutingTableEntryList* entryList = table[destination];
        for (auto& entry: *entryList) {
            if (entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                return entry->getPheromoneValue();
            }
        }
    }

    return 0;
}

bool RoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
    if (isDeliverable(destination)) {
        RoutingTableEntryList* entries = table[destination];
        for (auto& entry: *entries) {
            if (entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)){
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

        if (evaporationPolicy->isEvaporationNecessary(timeDifference)) {
            lastAccessTime->setToCurrentTime();

            RoutingTableMap::iterator i = table.begin();
            while (i!=table.end()) {
                std::pair<AddressPtr const, RoutingTableEntryList*> entryPair = *i;
                AddressPtr destination = entryPair.first;
                RoutingTableEntryList* entryList = entryPair.second;

                RoutingTableEntryList::iterator j = entryList->begin();
                while (j != entryList->end()) {
                    RoutingTableEntry* entry = *j;
                    float newPheromoneValue = evaporationPolicy->evaporate(entry->getPheromoneValue(), timeDifference);
                    if (newPheromoneValue > 0) {
                        entry->setPheromoneValue(newPheromoneValue);
                        j++;
                    }
                    else {
                        delete entry;
                        j = entryList->erase(j);
                    }
                }

                if (entryList->empty()) {
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
    RoutingTableMap::const_iterator iterator;
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

std::deque<RoutingTableEntryTupel> RoutingTable::getAllRoutesThatLeadOver(AddressPtr nextHop) const {
    std::deque<RoutingTableEntryTupel> result = std::deque<RoutingTableEntryTupel>();

    // TODO this could be made faster with an additional hashmap (but would require more memory)
    for (RoutingTableMap::const_iterator iterator=table.begin(); iterator!=table.end(); iterator++) {
        AddressPtr destination = iterator->first;
        RoutingTableEntryList* entryList = iterator->second;
        for (auto& entry: *entryList) {
            if(entry->getAddress()->equals(nextHop)) {
                RoutingTableEntryTupel tupel;
                tupel.destination = destination;
                tupel.entry = entry;
                result.push_back(tupel);
            }
        }
    }

    return result;
}

ARA_NAMESPACE_END
