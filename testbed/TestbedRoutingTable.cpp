/*
 * $FU-Copyright$
 */
#include "TestbedRoutingTable.h"

TESTBED_NAMESPACE_BEGIN

TestbedRoutingTable::TestbedRoutingTable() : RoutingTable() { }

TestbedRoutingTable::~TestbedRoutingTable() {}

void TestbedRoutingTable::update(AddressPtr destination, RoutingTableEntry* newEntry) {
    std::lock_guard<std::mutex> lock(mutex);
    RoutingTable::update(destination, newEntry);
}

float TestbedRoutingTable::getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    std::lock_guard<std::mutex> lock(mutex);
    float result = RoutingTable::getPheromoneValue(destination, nextHop, interface);
    return result;
}

void TestbedRoutingTable::removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    std::lock_guard<std::mutex> lock(mutex);
    RoutingTable::removeEntry(destination, nextHop, interface);
}

RoutingTableEntryList TestbedRoutingTable::getPossibleNextHops(const Packet* packet) {
    std::lock_guard<std::mutex> lock(mutex);
    RoutingTableEntryList result = RoutingTable::getPossibleNextHops(packet);
    return result;
}

RoutingTableEntryList TestbedRoutingTable::getPossibleNextHops(AddressPtr destination) {
    std::lock_guard<std::mutex> lock(mutex);
    RoutingTableEntryList result = RoutingTable::getPossibleNextHops(destination);
    return result;
}

bool TestbedRoutingTable::isDeliverable(AddressPtr destination) {
    std::lock_guard<std::mutex> lock(mutex);
    bool result = RoutingTable::isDeliverable(destination);
    return result;
}

bool TestbedRoutingTable::isDeliverable(const Packet* packet) {
    std::lock_guard<std::mutex> lock(mutex);
    bool result = RoutingTable::isDeliverable(packet);
    return result;
}

bool TestbedRoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
    std::lock_guard<std::mutex> lock(mutex);
    bool result = RoutingTable::exists(destination, nextHop, interface);
    return result;
}

void TestbedRoutingTable::triggerEvaporation() {
//    std::lock_guard<std::mutex> lock(mutex);
    Time* currentTime = Environment::getClock()->makeTime();
    currentTime->setToCurrentTime();

    if (lastAccessTime == nullptr) {
        lastAccessTime = currentTime;
    } else {
        applyEvaporation(currentTime);
    }
}

void TestbedRoutingTable::applyEvaporation(Time* currentTime){
    long timeDifference = currentTime->getDifferenceInMilliSeconds(lastAccessTime);
    delete currentTime;

    if (evaporationPolicy->isEvaporationNecessary(timeDifference)) {
        lastAccessTime->setToCurrentTime();

        std::lock_guard<std::mutex> lock(mutex);
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
                } else {
                    delete entry;
                    j = nextHopsForDestination->erase(j); // this does not invalidate the iterator, because j is set to the valid return value of erase (will point to end() if empty)
                }
            }

            if (nextHopsForDestination->empty()) {
                i = table.erase(i); // this does not invalidate the iterator, because i is set to the valid return value of erase (will point to end() if empty)
            } else {
                i++;
            }
        }
    }
}

unsigned int TestbedRoutingTable::getTotalNumberOfEntries() const {
    std::lock_guard<std::mutex> lock(mutex);
    unsigned int result = RoutingTable::getTotalNumberOfEntries();
    return result;
}

std::deque<RoutingTableEntryTupel> TestbedRoutingTable::getAllRoutesThatLeadOver(AddressPtr nextHop) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::deque<RoutingTableEntryTupel> result = RoutingTable::getAllRoutesThatLeadOver(nextHop);
    return result;
}

RoutingTableEntryTupel TestbedRoutingTable::getEntryAt(int wantedPosition) const {
    std::lock_guard<std::mutex> lock(mutex);
    RoutingTableEntryTupel result = RoutingTable::getEntryAt(wantedPosition);
    return result;
}

std::string TestbedRoutingTable::toString(int position) {
    std::lock_guard<std::mutex> lock(mutex);
    std::string result = RoutingTable::toString(position);
    return result;
}

TESTBED_NAMESPACE_END
