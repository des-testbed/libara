/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "RoutingTable.h"
#include "StochasticForwardingPolicy.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "PacketType.h"
#include "Exception.h" 
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/time/ClockMock.h"

#include <iostream>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(StochasticForwardingPolicyTest) {};

TEST(StochasticForwardingPolicyTest, testGetNextHop) {
    EvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicyMock();
    RoutingTable routingTable = RoutingTable(new ClockMock());
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr destination (new AddressMock("Destination"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // create multiple next hops
    AddressPtr nextHopA (new AddressMock("nextHopA"));
    AddressPtr nextHopB (new AddressMock("nextHopB"));
    AddressPtr nextHopC (new AddressMock("nextHopC"));

    PacketMock packet = PacketMock();

    // Start the test
    routingTable.update(destination, nextHopA, &interface, 1.2);
    routingTable.update(destination, nextHopB, &interface, 2.1);
    routingTable.update(destination, nextHopC, &interface, 2.3);

    StochasticForwardingPolicy policy = StochasticForwardingPolicy();
    policy.setRoutingTable(&routingTable);
    policy.getNextHop(&packet);
    //FIXME Continue this test

    delete evaporationPolicy;
}
