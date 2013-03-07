/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"

#include "LinearPathReinforcementPolicy.h"
#include "RoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "PacketType.h"
#include "Exception.h" 
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/time/ClockMock.h"

#include <iostream>
#include <memory>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(LinearPathReinforcementPolicyTest) {};

TEST(LinearPathReinforcementPolicyTest, pathReinforcementViaRoutingTableUpdate) {
    /// routing table and evaporation policy setup
    EvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicyMock();
    RoutingTable routingTable = RoutingTable(new ClockMock());
    routingTable.setEvaporationPolicy(evaporationPolicy);

    AddressPtr destination (new AddressMock("Destination"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr nextHop (new AddressMock("nextHop"));

    // start the test
    routingTable.update(destination, nextHop, &interface, 1.2);
    
    LinearPathReinforcementPolicy reinforcementPolicy = LinearPathReinforcementPolicy(&routingTable, 1.2);
    reinforcementPolicy.update(destination, nextHop, &interface);

    DOUBLES_EQUAL(routingTable.getPheromoneValue(destination, nextHop, &interface), 2.4,  0.00001);
    delete evaporationPolicy;
}
