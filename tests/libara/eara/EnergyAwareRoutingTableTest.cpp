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

TEST(EnergyAwareRoutingTableTest, update) {
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr nextHop (new AddressMock("nextHop"));

    float pheromoneValue1 = 123.456;
    float energyValue1 = 8.7;
    routingTable->update(destination, nextHop, interface, pheromoneValue1, energyValue1);
    DOUBLES_EQUAL(pheromoneValue1, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(energyValue1, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);

    float pheromoneValue2 = 323.456;
    float energyValue2 = 5.6;
    routingTable->update(destination, nextHop, interface, pheromoneValue2, energyValue2);
    DOUBLES_EQUAL(pheromoneValue2, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(energyValue2, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);
}

TEST(EnergyAwareRoutingTableTest, updateEnergyValue) {
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr nextHop (new AddressMock("nextHop"));

    float pheromoneValue = 123.456;
    routingTable->update(destination, nextHop, interface, pheromoneValue, 10);
    DOUBLES_EQUAL(pheromoneValue, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(10, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);

    float energyValue = 8.7;
    routingTable->updateEnergyValue(destination, nextHop, interface, energyValue);
    DOUBLES_EQUAL(pheromoneValue, routingTable->getPheromoneValue(destination, nextHop, interface), 0.0001);
    DOUBLES_EQUAL(energyValue, routingTable->getEnergyValue(destination, nextHop, interface), 0.0001);
}
