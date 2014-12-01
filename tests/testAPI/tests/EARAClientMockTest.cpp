/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/libara/EARAClientMock.h"
#include "EnergyAwareRoutingTable.h"

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EARAClientMockTest) {
    EARAClientMock* client;

    void setup() {
        client = new EARAClientMock();
    }

    void teardown() {
        delete client;
    }
};

TEST(EARAClientMockTest, getRoutingTable) {
    EnergyAwareRoutingTable* routingTable = client->getRoutingTable();
    CHECK(routingTable != NULL);
}

TEST(EARAClientMockTest, getCurrentEnergyLevel) {
    client->setEnergy(255);
    BYTES_EQUAL(255, client->getCurrentEnergyLevel());

    client->setEnergy(101);
    BYTES_EQUAL(101, client->getCurrentEnergyLevel());

    client->setEnergy(0);
    BYTES_EQUAL(0, client->getCurrentEnergyLevel());
}
