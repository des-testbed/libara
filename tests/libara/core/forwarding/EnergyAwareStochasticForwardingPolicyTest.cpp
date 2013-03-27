/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTable.h"
#include "NextHop.h"
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

    AddressPtr nextHopA (new AddressMock("A"));
    AddressPtr nextHopB (new AddressMock("B"));

    PacketMock packet = PacketMock();

    routingTable.update(destination, nextHopA, &interface, 1.2, 250);
    routingTable.update(destination, nextHopB, &interface, 2.1, 240);

    unsigned int randomNumberGeneratorSeed = 23; // the random number produced will be 0.727582
    EnergyAwareStochasticForwardingPolicyMock policy = EnergyAwareStochasticForwardingPolicyMock(randomNumberGeneratorSeed);
    policy.setPheromoneWeight(1.0);
    policy.setEnergyWeight(1.0);

    NextHop* result = policy.getNextHop(&packet, &routingTable);
    CHECK(result->getAddress()->equals(nextHopB));

    randomNumberGeneratorSeed = 42;
    policy = EnergyAwareStochasticForwardingPolicyMock(randomNumberGeneratorSeed);
    result = policy.getNextHop(&packet, &routingTable);
    CHECK(result->getAddress()->equals(nextHopA));

    delete evaporationPolicy;
}
