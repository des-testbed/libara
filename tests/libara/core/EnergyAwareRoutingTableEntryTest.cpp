/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EnergyAwareRoutingTableEntryTest) {};

TEST(EnergyAwareRoutingTableEntryTest, testGetters) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr nextHop(new AddressMock());
    float pheromoneValue = 1.234;
    float energyValue = 5.678;
    EnergyAwareRoutingTableEntry entry = EnergyAwareRoutingTableEntry(nextHop, &interface, pheromoneValue, energyValue);

    CHECK(nextHop->equals(entry.getAddress()));
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, entry.getPheromoneValue());
    CHECK_EQUAL(energyValue, entry.getEnergyValue());
}

TEST(EnergyAwareRoutingTableEntryTest, testSetPheromoneValue) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address(new AddressMock());
    float pheromoneValue = 1.234;
    float energyValue = 5.678;
    EnergyAwareRoutingTableEntry entry = EnergyAwareRoutingTableEntry(address, &interface, pheromoneValue, energyValue);

    entry.setPheromoneValue(42);
    CHECK(address->equals(entry.getAddress()));
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(42, entry.getPheromoneValue());
}

TEST(EnergyAwareRoutingTableEntryTest, testSetEnergyValue) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address(new AddressMock());
    float pheromoneValue = 1.234;
    float energyValue = 5.678;
    EnergyAwareRoutingTableEntry entry = EnergyAwareRoutingTableEntry(address, &interface, pheromoneValue, energyValue);

    entry.setEnergyValue(23);
    CHECK(address->equals(entry.getAddress()));
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, entry.getPheromoneValue());
    CHECK_EQUAL(23, entry.getEnergyValue());
}

TEST(EnergyAwareRoutingTableEntryTest, testGetNextHop) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address(new AddressMock());
    float pheromoneValue = 1.234;
    float energyValue = 5.678;
    EnergyAwareRoutingTableEntry entry = EnergyAwareRoutingTableEntry(address, &interface, pheromoneValue, energyValue);

    NextHop* nextHop = entry.getNextHop();
    CHECK(nextHop->getAddress()->equals(address));
    CHECK(nextHop->getInterface()->equals(&interface));
}
