/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableEntryTest) {};

TEST(RoutingTableEntryTest, testGetters) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr nextHop (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(nextHop, &interface, pheromoneValue);

    CHECK(nextHop->equals(entry.getAddress()));
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, entry.getPheromoneValue());
}

TEST(RoutingTableEntryTest, testOutputStreamOperator) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address(new AddressMock);
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, &interface, pheromoneValue);
    std::ostringstream stream;
    stream << entry;
    STRCMP_EQUAL("[next hop] Foo [phi] 1.234", stream.str().c_str());
}

TEST(RoutingTableEntryTest, testSetPheromoneValue) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, &interface, pheromoneValue);

    entry.setPheromoneValue(42);
    CHECK(address->equals(entry.getAddress()));
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(42, entry.getPheromoneValue());
}

TEST(RoutingTableEntryTest, testGetNextHop) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, &interface, pheromoneValue);

    NextHop* nextHop = entry.getNextHop();
    CHECK(nextHop->getAddress()->equals(address));
    CHECK(nextHop->getInterface()->equals(&interface));
}
