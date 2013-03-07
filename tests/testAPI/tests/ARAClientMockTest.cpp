/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "Address.h"
#include "PacketTrap.h"
#include "RoutingTable.h"

#include <deque>

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(ARAClientMockTest) {
    ARAClientMock* client;

    void setup() {
        client = new ARAClientMock();
    }

    void teardown() {
        delete client;
    }
};

TEST(ARAClientMockTest, getPacketTrap){
    PacketTrap* packetTrap = client->getPacketTrap();
    CHECK(packetTrap != NULL);
}

TEST(ARAClientMockTest, getRoutingTable){
    RoutingTable* routingTable = client->getRoutingTable();
    CHECK(routingTable != NULL);
}

TEST(ARAClientMockTest, getNetworkInterfaceMock) {
    CHECK_EQUAL(0, client->getNumberOfNetworkInterfaces());

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressMock defaultLocalAddress = AddressMock("DEFAULT");
    CHECK_EQUAL(1, client->getNumberOfNetworkInterfaces());
    CHECK_EQUAL("InterfaceMock1", interface->getName());
    CHECK(interface->getLocalAddress()->equals(&defaultLocalAddress));

    interface = client->createNewNetworkInterfaceMock("192.168.0.1");
    AddressMock expectedLocalAddress = AddressMock("192.168.0.1");
    CHECK_EQUAL(2, client->getNumberOfNetworkInterfaces());
    CHECK_EQUAL("InterfaceMock2", interface->getName());
    CHECK(interface->getLocalAddress()->equals(&expectedLocalAddress));
}

TEST(ARAClientMockTest, rememberPacketsThatHaveBeenDeliveredToTheSystem) {
    PacketMock packet1 = PacketMock("A", "B");
    PacketMock packet2 = PacketMock("B", "C");
    PacketMock packet3 = PacketMock("B", "C");

    std::deque<const Packet*>* deliveredPackets = client->getDeliveredPackets();
    CHECK(deliveredPackets->empty());

    client->deliverToSystem(&packet1);
    LONGS_EQUAL(1, deliveredPackets->size());
    CHECK(deliveredPackets->at(0)->equals(&packet1));

    client->deliverToSystem(&packet2);
    LONGS_EQUAL(2, deliveredPackets->size());
    CHECK(deliveredPackets->at(1)->equals(&packet2));

    client->deliverToSystem(&packet3);
    LONGS_EQUAL(3, deliveredPackets->size());
    CHECK(deliveredPackets->at(2)->equals(&packet3));
}

TEST(ARAClientMockTest, getNumberOfReceivedPackets) {
    Packet* packet = new PacketMock();
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();

    BYTES_EQUAL(0, client->getNumberOfReceivedPackets());
    client->receivePacket(packet, interface);
    BYTES_EQUAL(1, client->getNumberOfReceivedPackets());
}

TEST(ARAClientMockTest, getNumberOfUndeliverablePackets) {
    Packet* packet = new PacketMock();
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr nextHop (new AddressMock("foo"));

    BYTES_EQUAL(0, client->getNumberOfUndeliverablePackets());
    client->packetIsNotDeliverable(packet, nextHop, interface);
    BYTES_EQUAL(1, client->getNumberOfUndeliverablePackets());
}
