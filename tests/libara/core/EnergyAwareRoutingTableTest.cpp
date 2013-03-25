/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTable.h"
#include "ExponentialEvaporationPolicy.h"
#include "EnergyAwareRoutingTableEntry.h"
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

TEST(EnergyAwareRoutingTableTest, getEnergyValue) {
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nextHopAddress (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // Should be zero because there is no known route to this destination
    LONGS_EQUAL(0, routingTable->getPheromoneValue(destination, nextHopAddress, &interface));

    routingTable->update(destination, nextHopAddress, &interface, 123, 234);
    LONGS_EQUAL(234, routingTable->getEnergyValue(destination, nextHopAddress, &interface));
}
