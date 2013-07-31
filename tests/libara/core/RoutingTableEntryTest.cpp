/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ARAClientMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableEntryTest) {
    ARAClientMock* client;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new ARAClientMock();
        interface = new NetworkInterfaceMock("wlan0", client);
    }

    void teardown() {
        delete interface;
        delete client;
    }
};

TEST(RoutingTableEntryTest, testGetters) {
    AddressPtr nextHop (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(nextHop, interface, pheromoneValue);

    CHECK(nextHop->equals(entry.getAddress()));
    CHECK_EQUAL(interface, entry.getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, entry.getPheromoneValue());
}

TEST(RoutingTableEntryTest, testOutputStreamOperator) {
    AddressPtr address(new AddressMock);
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, interface, pheromoneValue);
    std::ostringstream stream;
    stream << entry;
    STRCMP_EQUAL("[next hop] Foo [phi] 1.234", stream.str().c_str());
}

TEST(RoutingTableEntryTest, testSetPheromoneValue) {
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, interface, pheromoneValue);

    entry.setPheromoneValue(42);
    CHECK(address->equals(entry.getAddress()));
    CHECK_EQUAL(interface, entry.getNetworkInterface());
    CHECK_EQUAL(42, entry.getPheromoneValue());
}

TEST(RoutingTableEntryTest, testGetNextHop) {
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, interface, pheromoneValue);

    NextHop* nextHop = entry.getNextHop();
    CHECK(nextHop->getAddress()->equals(address));
    CHECK(nextHop->getInterface()->equals(interface));
}
