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
#include "TestbedPacketFactory.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedARAClientTest) {
    TestbedARAClient* client;
    TestbedPacketFactory* packetFactory;

    void setup() {
        RoutingTableMock* routingTable = new RoutingTableMock();
        packetFactory = new TestbedPacketFactory(15);
        std::shared_ptr<PacketTrap> packetTrap = std::make_shared<PacketTrap>(routingTable);
        BasicConfiguration config = BasicConfiguration(routingTable, packetFactory, new ExponentialEvaporationPolicyMock(),
                                        new LinearPathReinforcementPolicy(5.0), new BestPheromoneForwardingPolicy(routingTable), packetTrap, 5.0);
        client = new TestbedARAClient(config);
    }

    void teardown() {
        /// TODO: check if that's a bright idea!
        Environment::setClock(nullptr);
        /**
         * we don't delete the packetFactory by ourselves, since the client
         * takes care of that
         */
        delete client;
    }
};

TEST(TestbedARAClientTest, configTest) {
    CHECK(client->getPacketFactory() != nullptr);
}
/*
TEST(TestbedARAClientTest, packetGetsTrappedIfNotDeliverable) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    PacketMock* packet = new PacketMock();
    packet->setSource(interface->getLocalAddress());

    CHECK(routingTable->isDeliverable(packet) == false);
    client->sendPacket(packet);
    CHECK(packetTrap->contains(packet));
}
*/

TEST(TestbedARAClientTest, getNextSequenceNumber) {
    for (unsigned int i = 1; i < 100; i++) {
        CHECK_EQUAL(i, client->getNextSequenceNumber());
    }
}

TESTBED_NAMESPACE_END
