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
    float energyValue = 8.5;
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
        STRCMP_EQUAL("Normalized energy values must be between 1 and 10", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 0.0);
        FAIL("Should have thrown an exception (energy = 0)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 1 and 10", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 0.5);
        FAIL("Should have thrown an exception (0 < energy < 1)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 1 and 10", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 10.05);
        FAIL("Should have thrown an exception (energy > 10)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 1 and 10", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 1000);
        FAIL("Should have thrown an exception (energy > 10)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 1 and 10", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 1);
        entry.setEnergyValue(10.23);
        FAIL("Should have thrown an exception (energy > 10)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 1 and 10", exception.getMessage());
    }

    try {
        EARARoutingTableEntry entry = EARARoutingTableEntry(nextHop, interface, 123, 8);
        entry.setEnergyValue(-0.3);
        FAIL("Should have thrown an exception (energy < 0)");
    } catch(Exception &exception) {
        STRCMP_EQUAL("Normalized energy values must be between 1 and 10", exception.getMessage());
    }
}

TEST(EARARoutingTableEntryTest, testOutputStreamOperator) {
    AddressPtr address(new AddressMock);
    float pheromoneValue = 1.234;
    float energyValue = 8.75;
    EARARoutingTableEntry entry = EARARoutingTableEntry(address, interface, pheromoneValue, energyValue);
    std::ostringstream stream;
    stream << entry;
    STRCMP_EQUAL("[next hop] Foo [phi] 1.234 [energy] 87.5%", stream.str().c_str());
}


TEST(EARARoutingTableEntryTest, testSetEnergyValue) {
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    float energyValue = 8.75;
    EARARoutingTableEntry entry = EARARoutingTableEntry(address, interface, pheromoneValue, energyValue);

    entry.setEnergyValue(6.123);
    DOUBLES_EQUAL(6.123, entry.getEnergyValue(), 0.0001);
}
