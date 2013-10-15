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
    try {
        if (table.find(destination) == table.end()) {
            // this is a new entry
            RoutingTableEntryList* entryList = new RoutingTableEntryList();
            entryList->push_back(newEntry);
            table[destination] = entryList;
        } else {
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
            } else {
                delete newEntry;
            }
        }
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::updateExistingEntry: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
    }
    
}

void RoutingTable::updateExistingEntry(RoutingTableEntry* oldEntry, RoutingTableEntry* newEntry){
    try {
        oldEntry->setPheromoneValue(newEntry->getPheromoneValue());
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::updateExistingEntry: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
    }
}

void RoutingTable::removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    try {
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
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::removeEntry: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
    }
}

RoutingTableEntryList RoutingTable::getPossibleNextHops(const Packet* packet) {
    try {
        if (isDeliverable(packet->getDestination())) {
            AddressPtr source = packet->getSource();
            AddressPtr sender = packet->getSender();

            RoutingTableEntryList* availableHops = table[packet->getDestination()];
            RoutingTableEntryList returnedList = RoutingTableEntryList(*availableHops);

            // remove all entries that would route the packet back over the source or sender of the packet (would create a loop)
            RoutingTableEntryList::iterator iterator = returnedList.begin();
            while (iterator != returnedList.end()) {
                RoutingTableEntry* entry = *iterator;
                AddressPtr possibleNextHop = entry->getAddress();
                if (possibleNextHop->equals(source) || possibleNextHop->equals(sender) ) {
                    iterator = returnedList.erase(iterator);
                } else {
                    iterator++;
                }
            }

            return returnedList;
        }
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::getPossibleNextHops: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
    }

    return RoutingTableEntryList();
}

RoutingTableEntryList RoutingTable::getPossibleNextHops(AddressPtr destination) {
    try {
        if (isDeliverable(destination)) {
            return *(table[destination]);
        } 
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::getPossibleNextHops: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
    }
    
    return RoutingTableEntryList();
}

bool RoutingTable::isDeliverable(AddressPtr destination) {
    return table.find(destination) != table.end();
}

bool RoutingTable::isDeliverable(const Packet* packet) {
    RoutingTableEntryList possibleNextHops = getPossibleNextHops(packet);
    return possibleNextHops.empty() == false;
}

float RoutingTable::getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    try {
        if (isDeliverable(destination)) {
            RoutingTableEntryList* entryList = table[destination];
            for (auto& entry: *entryList) {
                if (entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                    return entry->getPheromoneValue();
                }
            }
        }
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::getPheromoneValue: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
    }

    return 0;
}

bool RoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
    try {
        if (isDeliverable(destination)) {
            RoutingTableEntryList* entries = table[destination];
            for (auto& entry: *entries) {
                if (entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)){
                    return true;
                }
            }
        }
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::exists: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
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
    } else {
        applyEvaporation(currentTime);
    }
}

void RoutingTable::applyEvaporation(Time* currentTime) {
    long timeDifference = currentTime->getDifferenceInMilliSeconds(lastAccessTime);
    delete currentTime;

    try {
        if (evaporationPolicy->isEvaporationNecessary(timeDifference)) {
            lastAccessTime->setToCurrentTime();

            RoutingTableMap::iterator i = table.begin();
            while (i!=table.end()) {
                std::pair<AddressPtr const, RoutingTableEntryList*> entryPair = *i;
                AddressPtr destination = entryPair.first;
                RoutingTableEntryList* nextHopsForDestination = entryPair.second;

                // apply evaporation to all next hops for that destination
                RoutingTableEntryList::iterator j = nextHopsForDestination->begin();
                while (j != nextHopsForDestination->end()) {
                    RoutingTableEntry* entry = *j;
                    float newPheromoneValue = evaporationPolicy->evaporate(entry->getPheromoneValue(), timeDifference);
                    if (newPheromoneValue > 0) {
                        entry->setPheromoneValue(newPheromoneValue);
                        j++;
                    } else {
                        delete entry;
                        j = nextHopsForDestination->erase(j); // this does not invalidate the iterator, because j is set to the valid return value of erase (will point to end() if empty)
                    }
                }

                if (nextHopsForDestination->empty()) {
                    delete nextHopsForDestination;
                    i = table.erase(i); // this does not invalidate the iterator, because i is set to the valid return value of erase (will point to end() if empty)
                } else {
                    i++;
                }
            }
        }
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::applyEvaporationTime: A out of range error occured! Reason was: " << rangeError.what() << std::endl;
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

    try {
        RoutingTableMap::const_iterator iterator;
        for (iterator=table.begin(); iterator!=table.end(); iterator++) {
            RoutingTableEntryList* entryList = iterator->second;
            tableSize += entryList->size();
        }
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::getTotalNumberOfEntries: A out of range error occured! Reason was: " << rangeError.what() << std::endl;
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
            } else {
                currentPosition++;
            }
        }
    }

    throw Exception("RoutingTable::getEntryAt: Index out of bounds");
}

std::deque<RoutingTableEntryTupel> RoutingTable::getAllRoutesThatLeadOver(AddressPtr nextHop) const {
    std::deque<RoutingTableEntryTupel> result;

    try {
        // TODO this could be made faster with an additional hashmap (but would require more memory)
        for (RoutingTableMap::const_iterator iterator=table.begin(); iterator!=table.end(); iterator++) {
            AddressPtr destination = iterator->first;
            RoutingTableEntryList* nextHopsForDestination = iterator->second;

            for (auto& entry: *nextHopsForDestination) {
                if (entry->getAddress()->equals(nextHop)) {
                    RoutingTableEntryTupel tupel;
                    tupel.destination = destination;
                    tupel.entry = entry;
                    result.push_back(tupel);
                }
            }
        }
    } catch (const std::out_of_range& rangeError) {
        std::cerr << "RoutingTable::getAllRoutesThatLeadOver: An out of range error occured! Reason was: " << rangeError.what() << std::endl;
    }

    return result;
}

ARA_NAMESPACE_END
