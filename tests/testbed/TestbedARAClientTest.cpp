/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "Testbed.h"
#include "TestbedARAClient.h"
#include "TestbedPacketFactory.h"

#include "testAPI/mocks/testbed/TestbedARAClientMock.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedARAClientTest) {
    TestbedARAClientMock* client;
    std::shared_ptr<TestbedPacketTrap> packetTrap;
    //RoutingTableMock* routingTable;
    TestbedPacketFactory* packetFactory;

    void setup() {
        client = new TestbedARAClientMock();
/*
        packetTrap = client->getPacketTrap();
        routingTable = (RoutingTableMock*) client->getRoutingTable();
        packetFactory = client->getPacketFactory();
*/
    }

    void teardown() {
        delete client;
    }

};


TESTBED_NAMESPACE_END
