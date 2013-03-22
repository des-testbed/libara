/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/EARAClientMock.h"
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
