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
#include "testAPI/mocks/StochasticForwardingPolicyMock.h"

#include <iostream>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(StochasticForwardingPolicyTest) {};

TEST(StochasticForwardingPolicyTest, testGetNextHop) {
    EvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicyMock();
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr destination (new AddressMock("Destination"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    AddressPtr nextHopA (new AddressMock("nextHopA"));
    AddressPtr nextHopB (new AddressMock("nextHopB"));

    PacketMock packet = PacketMock();

    routingTable.update(destination, nextHopA, &interface, 1.2);
    routingTable.update(destination, nextHopB, &interface, 2.1);

    StochasticForwardingPolicyMock policy = StochasticForwardingPolicyMock(42);

    NextHop* result = policy.getNextHop(&packet, &routingTable);
    CHECK(result->getAddress()->equals(nextHopA));

    delete evaporationPolicy;
}
