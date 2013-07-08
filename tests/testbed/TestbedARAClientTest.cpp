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

#include "TestbedARAClient.h"


TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedARAClientTest) {
    TestbedARAClient* client;

    void setup() {
        BasicConfiguration config = BasicConfiguration(new RoutingTableMock(), new PacketFactory(15), new ExponentialEvaporationPolicyMock(),
                                        new LinearPathReinforcementPolicy(5.0), new BestPheromoneForwardingPolicy(), 5.0);
        client = new TestbedARAClient(config);
    }

    void teardown() {
        delete client;
    }
};

TEST(TestbedARAClientTest, configTest) {
    CHECK(client->getPacketFactory() != nullptr);
}

TESTBED_NAMESPACE_END
