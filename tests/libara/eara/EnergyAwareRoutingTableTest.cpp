/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTable.h"
#include "testAPI/mocks/EARAClientMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/AddressMock.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EnergyAwareRoutingTableTest) {
    EARAClientMock* client;
    EnergyAwareRoutingTable* routingTable;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new EARAClientMock();
        routingTable = client->getRoutingTable();
        interface = client->createNewNetworkInterfaceMock();
    }

    void teardown() {
        delete client;
    }
};

/**
 * This test checks, that the energy fitness equals to 1 if it has not yet
 * been set. Because this value will later be multiplied with the TTL values
 * this means that an unset (unknown) energy value will not influence the result.
 * Theoretically it should never happen in EARA that a route exists without a known
 * energy value but we better make sure everything would still work.
 */
TEST(EnergyAwareRoutingTableTest, energyFitnessEqualsOneIfNotSet) {
    AddressPtr someDestination (new AddressMock("destination"));
    AddressPtr someNextHop (new AddressMock("foo"));
    LONGS_EQUAL(1, routingTable->getEnergyValue(someNextHop, someDestination, interface));
}

TEST(EnergyAwareRoutingTableTest, updateEnergyValue) {
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr nextHop (new AddressMock("nextHop"));

    float pheromoneValue1 = 123.456;
    float energyValue1 = 0.87;
    routingTable->update(destination, nextHop, interface, pheromoneValue1, energyValue1);
    DOUBLES_EQUAL(pheromoneValue1, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(energyValue1, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);

    float pheromoneValue2 = 323.456;
    float energyValue2 = 0.56;
    routingTable->update(destination, nextHop, interface, pheromoneValue2, energyValue2);
    DOUBLES_EQUAL(pheromoneValue2, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(energyValue2, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);
}

TEST(EnergyAwareRoutingTableTest, updatePheromoneValueWithoutAffectingEnergyValue) {
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr nextHop (new AddressMock("nextHop"));

    float energyValue = 0.87;
    routingTable->update(destination, nextHop, interface, 10, energyValue);
    DOUBLES_EQUAL(energyValue, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);

    routingTable->update(destination, nextHop, interface, 20);
    DOUBLES_EQUAL(energyValue, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);
}

TEST(EnergyAwareRoutingTableTest, updateOnlyEnergyValue) {
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr nextHop (new AddressMock("nextHop"));

    float pheromoneValue = 123.456;
    routingTable->update(destination, nextHop, interface, pheromoneValue);
    DOUBLES_EQUAL(pheromoneValue, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(1.0, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);

    float energyValue = 0.87;
    routingTable->updateEnergyValue(destination, nextHop, interface, energyValue);
    DOUBLES_EQUAL(pheromoneValue, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(energyValue, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);
}
