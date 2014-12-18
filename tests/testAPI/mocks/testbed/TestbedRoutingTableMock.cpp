/*
 * $FU-Copyright$
 */

#include "TestbedRoutingTableMock.h"

TESTBED_NAMESPACE_BEGIN

RoutingTableEntryList TestbedRoutingTableMock::getPossibleNextHops(AddressPtr destination) {
    return *(table[destination]);
}

//void TestbedRoutingTableMock::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue) {
//TestbedRoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue) {
TESTBED_NAMESPACE_END
