/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "TestMacros.h"
#include "PacketTrap.h"
#include "EnergyAwareRoutingTable.h"
#include "Environment.h"
#include "Pair.h"
#include "PacketType.h"

#include "testAPI/mocks/EARAClientMock.h"
#include "testAPI/mocks/time/TimerMock.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(AbstractEARAClientTest) {
    EARAClientMock* client;
    PacketTrap* packetTrap;
    EnergyAwareRoutingTable* routingTable;
    PacketFactory* packetFactory;

    void setup() {
        client = new EARAClientMock();
        packetTrap = client->getPacketTrap();
        routingTable = client->getRoutingTable();
        packetFactory = Environment::getPacketFactory();
    }

    void teardown() {
        delete client;
    }
};

TEST(AbstractEARAClientTest, sendPeriodicEnergyInformationMessages) {
    // prepare the test
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("source");
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();
    TimerMock* timer = client->getEnergyDisseminationTimer();

    // sanity check
    BYTES_EQUAL(0, sentPackets->size());

    // start the test
    CHECK(timer->isRunning());

    // let the timer expire and check if the client has broadcasted the HELLO packet
    timer->expire();

    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->back();
    const Packet* sentPacket = sentPacketInfo->getLeft();

    CHECK(interface->isBroadcastAddress(sentPacketInfo->getRight()));
    CHECK(sentPacket->getType() == PacketType::ENERGY_INFO);
    CHECK(sentPacket->getSource()->equals(interface->getLocalAddress()));

    // check the payload
    BYTES_EQUAL(1, sentPacket->getPayloadLength());
    const char* payload = sentPacket->getPayload();
    BYTES_EQUAL(client->getCurrentEnergyLevel(), payload[0]);
}

TEST(AbstractEARAClientTest, processPeriodicEnergyInformationMessages) {
    // prepare the test
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    AddressPtr someNode (new AddressMock("B"));
    AddressPtr destination (new AddressMock("C"));
    unsigned int sequenceNumber = 123;
    unsigned char energyLevel = 140;
    Packet* energyPacket = packetFactory->makeEnergyDisseminationPacket(someNode, sequenceNumber, energyLevel);
    routingTable->updateEnergyOfNode(someNode, 255);

    // sanity check
    BYTES_EQUAL(255, routingTable->getEnergyValueOf(someNode));

    // start the test
    client->receivePacket(energyPacket, interface);
    BYTES_EQUAL(energyLevel, routingTable->getEnergyValueOf(someNode));
}
