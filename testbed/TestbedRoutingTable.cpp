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

/*
void TestbedRoutingTable::applyEvaporation(Time* currentTime) {
    std::lock_guard<std::mutex> lock(mutex);
    RoutingTable::applyEvaporation(currentTime);
}
*/

float TestbedRoutingTable::getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    std::unique_lock<std::mutex> lock(mutex);
    float result = RoutingTable::getPheromoneValue(destination, nextHop, interface);
    return result;
}

void TestbedRoutingTable::removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    std::unique_lock<std::mutex> lock(mutex);
    RoutingTable::removeEntry(destination, nextHop, interface);
}

RoutingTableEntryList TestbedRoutingTable::getPossibleNextHops(const Packet* packet) {
    std::unique_lock<std::mutex> lock(mutex);
    RoutingTableEntryList result = RoutingTable::getPossibleNextHops(packet);
    return result;
}

RoutingTableEntryList TestbedRoutingTable::getPossibleNextHops(AddressPtr destination) {
    std::unique_lock<std::mutex> lock(mutex);
    RoutingTableEntryList result = RoutingTable::getPossibleNextHops(destination);
    return result;
}

bool TestbedRoutingTable::isDeliverable(AddressPtr destination) {
    std::unique_lock<std::mutex> lock(mutex);
    bool result = RoutingTable::isDeliverable(destination);
    return result;
}

bool TestbedRoutingTable::isDeliverable(const Packet* packet) {
    std::unique_lock<std::mutex> lock(mutex);
    bool result = RoutingTable::isDeliverable(packet);
    return result;
}

bool TestbedRoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
    std::unique_lock<std::mutex> lock(mutex);
    bool result = RoutingTable::exists(destination, nextHop, interface);
    return result;
}

void TestbedRoutingTable::triggerEvaporation() {
    std::unique_lock<std::mutex> lock(mutex);
    RoutingTable::triggerEvaporation();
}

unsigned int TestbedRoutingTable::getTotalNumberOfEntries() const {
    std::unique_lock<std::mutex> lock(mutex);
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

TESTBED_NAMESPACE_END
