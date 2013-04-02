/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTable.h"
#include "ExponentialEvaporationPolicy.h"
#include "PacketType.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/time/TimeMock.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EnergyAwareRoutingTableTest) {
    EnergyAwareRoutingTable* routingTable;
    ExponentialEvaporationPolicyMock* evaporationPolicy;

    void setup() {
        evaporationPolicy = new ExponentialEvaporationPolicyMock();
        routingTable = new EnergyAwareRoutingTable();
        routingTable->setEvaporationPolicy(evaporationPolicy);
    }

    void teardown() {
        delete routingTable;
        delete evaporationPolicy;
    }
};

TEST(EnergyAwareRoutingTableTest, updateEnergyOfNode) {
    AddressPtr node1 (new AddressMock("A"));
    AddressPtr node2 (new AddressMock("B"));

    // start the test
    unsigned char initialEnergyValue = 255;
    routingTable->updateEnergyOfNode(node1, initialEnergyValue);
    routingTable->updateEnergyOfNode(node2, initialEnergyValue);
    BYTES_EQUAL(initialEnergyValue, routingTable->getEnergyValueOf(node1));
    BYTES_EQUAL(initialEnergyValue, routingTable->getEnergyValueOf(node2));

    unsigned char newEnergyValue1 = 120;
    routingTable->updateEnergyOfNode(node1, newEnergyValue1);

    BYTES_EQUAL(newEnergyValue1, routingTable->getEnergyValueOf(node1));
    BYTES_EQUAL(initialEnergyValue, routingTable->getEnergyValueOf(node2));

    unsigned char newEnergyValue2 = 55;
    routingTable->updateEnergyOfNode(node2, newEnergyValue2);

    BYTES_EQUAL(newEnergyValue1, routingTable->getEnergyValueOf(node1));
    BYTES_EQUAL(newEnergyValue2, routingTable->getEnergyValueOf(node2));
}

TEST(EnergyAwareRoutingTableTest, hasEnergyInformationFor) {
    AddressPtr someNode (new AddressMock("A"));
    CHECK_FALSE(routingTable->hasEnergyInformationFor(someNode));
    routingTable->updateEnergyOfNode(someNode, 123);
    CHECK_TRUE(routingTable->hasEnergyInformationFor(someNode));
}
