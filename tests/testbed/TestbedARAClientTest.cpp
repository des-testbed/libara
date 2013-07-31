/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"
#include "BasicConfiguration.h"
#include "testAPI/mocks/RoutingTableMock.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "LinearPathReinforcementPolicy.h"
#include "BestPheromoneForwardingPolicy.h"
#include "Environment.h"
#include "TestbedARAClient.h"


TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedARAClientTest) {
    TestbedARAClient* client;
    RoutingTableMock* routingTable;

    void setup() {
	routingTable = new RoutingTableMock();
        BasicConfiguration config = BasicConfiguration(routingTable, new PacketFactory(15), new ExponentialEvaporationPolicyMock(), new LinearPathReinforcementPolicy(5.0), new BestPheromoneForwardingPolicy(routingTable), 5.0);
        client = new TestbedARAClient(config);
    }

    void teardown() {
        Environment::setClock(nullptr);
        delete client;
    }
};

TEST(TestbedARAClientTest, configTest) {
    CHECK(client->getPacketFactory() != nullptr);
}

TESTBED_NAMESPACE_END
