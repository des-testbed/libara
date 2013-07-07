/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EARARoutingTableEntry.h"
#include "NextHop.h"
#include "Exception.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ARAClientMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EARARoutingTableEntryTest) {
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

TEST(EARARoutingTableEntryTest, testGetters) {
    AddressPtr nextHop (new AddressMock());
    float pheromoneValue = 1.234;
    float energyValue = 0.8;
    EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, pheromoneValue, energyValue);

    CHECK(nextHop->equals(entry.getAddress()));
    CHECK_EQUAL(interface, entry.getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, entry.getPheromoneValue());
    CHECK_EQUAL(energyValue, entry.getEnergyValue());
}

TEST(EARARoutingTableEntryTest, testOnlyAcceptNormalizedEnergy) {
    AddressPtr nextHop (new AddressMock());
    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, -1);
        FAIL("Should have thrown an exception (energy < 0)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 0 and 1", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 1.05);
        FAIL("Should have thrown an exception (energy < 0)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 0 and 1", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 10);
        FAIL("Should have thrown an exception (energy < 0)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 0 and 1", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 1);
        entry.setEnergyValue(1.23);
        FAIL("Should have thrown an exception (energy < 0)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 0 and 1", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 1);
        entry.setEnergyValue(-0.3);
        FAIL("Should have thrown an exception (energy < 0)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 0 and 1", exception.getMessage());
    }
}

TEST(EARARoutingTableEntryTest, testOutputStreamOperator) {
    AddressPtr address(new AddressMock);
    float pheromoneValue = 1.234;
    float energyValue = 0.875;
    EARARoutingTableEntry entry = EARARoutingTableEntry(address, interface, pheromoneValue, energyValue);
    std::ostringstream stream;
    stream << entry;
    STRCMP_EQUAL("[next hop] Foo [phi] 1.234 [energy] 0.875", stream.str().c_str());
}


TEST(EARARoutingTableEntryTest, testSetEnergyValue) {
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    float energyValue = 0.875;
    EARARoutingTableEntry entry = EARARoutingTableEntry(address, interface, pheromoneValue, energyValue);

    entry.setEnergyValue(0.6123);
    DOUBLES_EQUAL(0.6123, entry.getEnergyValue(), 0.0001);
}
