/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTable.h"
#include "EvaporationPolicy.h"
#include "NextHop.h"
#include "testAPI/mocks/EARAClientMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/EnergyAwareStochasticForwardingPolicyMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EnergyAwareStochasticForwardingPolicyTest) {
    EARAClientMock* client;
    EnergyAwareRoutingTable* routingTable;
    EvaporationPolicy* evaporationPolicy;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new EARAClientMock();
        routingTable = client->getRoutingTable();
        evaporationPolicy = routingTable->getEvaporationPolicy();
        interface = client->createNewNetworkInterfaceMock();
    }

    void teardown() {
        delete client;
    }
};

TEST(EnergyAwareStochasticForwardingPolicyTest, testGetNextHop) {
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nextHopA (new AddressMock("A"));
    AddressPtr nextHopB (new AddressMock("B"));

    PacketMock packet = PacketMock();

    routingTable->update(destination, nextHopA, interface, 1.2, 0.8);
    routingTable->update(destination, nextHopB, interface, 2.1, 0.6);

    unsigned int randomNumberGeneratorSeed = 23; // the random number produced will be 0.727582
    EnergyAwareStochasticForwardingPolicyMock policy = EnergyAwareStochasticForwardingPolicyMock(routingTable, randomNumberGeneratorSeed);
    policy.setPheromoneWeight(1.0);
    policy.setEnergyWeight(1.0);

    NextHop* result = policy.getNextHop(&packet);
    CHECK(result->getAddress()->equals(nextHopB));

    randomNumberGeneratorSeed = 42;
    policy = EnergyAwareStochasticForwardingPolicyMock(routingTable, randomNumberGeneratorSeed);
    result = policy.getNextHop(&packet);
    CHECK(result->getAddress()->equals(nextHopA));
}
