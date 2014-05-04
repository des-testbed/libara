/*
 * $FU-Copyright$
 */

#include "RoutingTable.h"
#include "Environment.h"
#include "Exception.h"

ARA_NAMESPACE_BEGIN

RoutingTable::RoutingTable() {
    lastAccessTime = nullptr;
    evaporationPolicy = nullptr;
}

RoutingTable::~RoutingTable() {
    RoutingTableMap::iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
	    std::shared_ptr<RoutingTableEntryList> entryList = iterator->second;

        // delete all RoutingTableEntries in the List
        while (entryList->empty() == false) {
            RoutingTableEntry* entry = entryList->back();
            entryList->pop_back();
            delete entry;
        }
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
	std::shared_ptr<RoutingTableEntryList> entryList = std::make_shared<RoutingTableEntryList>();
        entryList->push_back(newEntry);
        table[destination] = entryList;
    }
    else {
        // there is at least one registered route for this destination
	std::shared_ptr<RoutingTableEntryList> entryList = table[destination];
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
	std::shared_ptr<RoutingTableEntryList> entryList = table[destination];
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
            }
        }
    }
}

RoutingTableEntryList RoutingTable::getPossibleNextHops(const Packet* packet) {
    if (isDeliverable(packet->getDestination())) {
        AddressPtr source = packet->getSource();
        AddressPtr sender = packet->getSender();

	std::shared_ptr<RoutingTableEntryList> availableHops = table[packet->getDestination()];
        RoutingTableEntryList returnedList = RoutingTableEntryList(*(availableHops));

        // remove all entries that would route the packet back over the source or sender of the packet (would create a loop)
        RoutingTableEntryList::iterator iterator = returnedList.begin();
        while (iterator != returnedList.end()) {
            RoutingTableEntry* entry = *iterator;
            AddressPtr possibleNextHop = entry->getAddress();
            if (possibleNextHop->equals(source) || possibleNextHop->equals(sender) ) {
                iterator = returnedList.erase(iterator);
            }
            else {
                iterator++;
            }
        }

        return returnedList;
    }
    else {
        // return empty list
        return RoutingTableEntryList();
    }
}

RoutingTableEntryList RoutingTable::getPossibleNextHops(AddressPtr destination) {
    if (isDeliverable(destination)) {
	std::shared_ptr<RoutingTableEntryList> list = table[destination];
        RoutingTableEntryList result = RoutingTableEntryList(*(list));
        return result;
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
    RoutingTableEntryList possibleNextHops = getPossibleNextHops(packet);
    return possibleNextHops.empty() == false;
}

float RoutingTable::getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    if (isDeliverable(destination)) {
	std::shared_ptr<RoutingTableEntryList> entryList = table[destination];
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
	std::shared_ptr<RoutingTableEntryList> entries = table[destination];
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
        applyEvaporation(currentTime);
    }
}

void RoutingTable::applyEvaporation(Time* currentTime) {
    long timeDifference = currentTime->getDifferenceInMilliSeconds(lastAccessTime);
    delete currentTime;

    if (evaporationPolicy->isEvaporationNecessary(timeDifference)) {
        lastAccessTime->setToCurrentTime();

        RoutingTableMap::iterator i = table.begin();
        while (i!=table.end()) {
            std::pair<AddressPtr const, std::shared_ptr<RoutingTableEntryList> > entryPair = *i;
            AddressPtr destination = entryPair.first;
	    std::shared_ptr<RoutingTableEntryList> nextHopsForDestination = entryPair.second;

            // apply evaporation to all next hops for that destination
            RoutingTableEntryList::iterator j = nextHopsForDestination->begin();
            while (j != nextHopsForDestination->end()) {
                RoutingTableEntry* entry = *j;
                float newPheromoneValue = evaporationPolicy->evaporate(entry->getPheromoneValue(), timeDifference);
                if (newPheromoneValue > 0) {
                    entry->setPheromoneValue(newPheromoneValue);
                    j++;
                }
                else {
                    delete entry;
                    j = nextHopsForDestination->erase(j); // this does not invalidate the iterator, because j is set to the valid return value of erase (will point to end() if empty)
                }
            }

            if (nextHopsForDestination->empty()) {
                i = table.erase(i); // this does not invalidate the iterator, because i is set to the valid return value of erase (will point to end() if empty)
            }
            else {
                i++;
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
    RoutingTableMap::const_iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
	std::shared_ptr<RoutingTableEntryList> entryList = iterator->second;
        tableSize += entryList->size();
    }
    return tableSize;
}

RoutingTableEntryTupel RoutingTable::getEntryAt(int wantedPosition) const {
    int currentPosition = 0;
    RoutingTableMap::const_iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        AddressPtr destination = iterator->first;
	std::shared_ptr<RoutingTableEntryList> entryList = iterator->second;
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
	std::shared_ptr<RoutingTableEntryList> nextHopsForDestination = iterator->second;
        for (auto& entry: *nextHopsForDestination) {
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

std::string RoutingTable::toString(int position) {
    int currentPosition = 0;
    std::ostringstream result;
    RoutingTableMap::const_iterator i;

    for (i = table.begin(); i !=table.end(); i++) {
        AddressPtr destination = i->first;
    	std::shared_ptr<RoutingTableEntryList> entryList = i->second;

        for (auto& entry: *entryList) {
	        if (position < 0) {
	            result << "[destination] " << *destination << " " << *entry << std::endl;
	        } else {
	            if (currentPosition == position) {
	                result << "[destination] " << *destination << " " << *entry << std::endl;
		            return result.str();
	            } else {
	                currentPosition++;
	            }
            }
        }
    }

    return result.str();
}

ARA_NAMESPACE_END
