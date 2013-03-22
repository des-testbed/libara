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

/// the method tests the overloaded output stream operator
TEST(EnergyAwareRoutingTableEntryTest, testOutputStreamOperator) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    std::shared_ptr<AddressMock> address(new AddressMock);
    EnergyAwareRoutingTableEntry entry = EnergyAwareRoutingTableEntry(address, &interface, 1.234, 5.678);
    std::ostringstream stream;
    stream << entry;
    STRCMP_EQUAL(stream.str().c_str(), "[next hop] Foo [phi] 1.234 [energy] 5.678");
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
