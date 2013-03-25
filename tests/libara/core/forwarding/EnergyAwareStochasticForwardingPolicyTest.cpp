/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTable.h"
#include "EnergyAwareStochasticForwardingPolicy.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "PacketType.h"
#include "Exception.h" 
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/EnergyAwareStochasticForwardingPolicyMock.h"

#include <iostream>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EnergyAwareStochasticForwardingPolicyTest) {};

TEST(EnergyAwareStochasticForwardingPolicyTest, testGetNextHop) {
    EvaporationPolicy* evaporationPolicy = new ExponentialEvaporationPolicyMock();
    EnergyAwareRoutingTable routingTable = EnergyAwareRoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr destination (new AddressMock("Destination"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    AddressPtr nextHopA (new AddressMock("nextHopA"));
    AddressPtr nextHopB (new AddressMock("nextHopB"));

    PacketMock packet = PacketMock();

    routingTable.update(destination, nextHopA, &interface, 1.2, 0.8);
    routingTable.update(destination, nextHopB, &interface, 2.1, 0.6);

    EnergyAwareStochasticForwardingPolicyMock policy = EnergyAwareStochasticForwardingPolicyMock(23);
    NextHop* result = policy.getNextHop(&packet, &routingTable);
    CHECK(result->getAddress()->equals(nextHopB));

    policy = EnergyAwareStochasticForwardingPolicyMock(42);
    result = policy.getNextHop(&packet, &routingTable);
    CHECK(result->getAddress()->equals(nextHopA));

    delete evaporationPolicy;
}
