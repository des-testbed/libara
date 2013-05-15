/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/ARAClientMock.h"
#include "Pair.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(NetworkInterfaceMockTest) {
    NetworkInterfaceMock* interface;

    void setup() {
        interface = new NetworkInterfaceMock("wlan0");
    }

    void teardown() {
        delete interface;
    }
};

TEST(NetworkInterfaceMockTest, mockRemembersSentPackets) {
    AddressPtr recipient1 (new AddressMock());
    AddressPtr recipient2 (new AddressMock());
    Packet* packet1 = new PacketMock();
    Packet* packet2 = new PacketMock();
    Packet* packet3 = new PacketMock();

    interface->send(packet1, recipient1);
    interface->send(packet2, recipient2);
    interface->send(packet3, recipient1);

    std::deque<Pair<const Packet*, AddressPtr>*>* sendPackets = interface->getSentPackets();

    CHECK(sendPackets->at(0)->getLeft()->equals(packet1));
    CHECK(sendPackets->at(0)->getRight()->equals(recipient1));

    CHECK(sendPackets->at(1)->getLeft()->equals(packet2));
    CHECK(sendPackets->at(1)->getRight()->equals(recipient2));

    CHECK(sendPackets->at(2)->getLeft()->equals(packet3));
    CHECK(sendPackets->at(2)->getRight()->equals(recipient1));
}

TEST(NetworkInterfaceMockTest, hasPacketBeenBroadCasted) {
    Packet* packet1 = new PacketMock("Source", "Destination", 1);
    Packet* packet2 = new PacketMock("Earth", "Moon", 1234);
    Packet* packet3 = new PacketMock("Source", "Destination", 2);
    AddressPtr viaAddress (new AddressMock());

    interface->send(packet1, viaAddress);
    interface->broadcast(packet2);
    interface->send(packet3, viaAddress);

    CHECK(interface->hasPacketBeenBroadCasted(packet1) == false);
    CHECK(interface->hasPacketBeenBroadCasted(packet2) == true);
    CHECK(interface->hasPacketBeenBroadCasted(packet3) == false);
}

TEST(NetworkInterfaceMockTest, hasPacketBeenSent) {
    Packet* packet1 = new PacketMock("Source", "Destination", 1);
    Packet* packet2 = new PacketMock("Source", "Destination", 2);
    Packet* packet3 = new PacketMock("Source", "Destination", 3);
    AddressPtr address (new AddressMock());

    interface->send(packet1, address);
    interface->broadcast(packet2);

    CHECK(interface->hasPacketBeenSent(packet1) == true);
    CHECK(interface->hasPacketBeenSent(packet2) == true);
    CHECK(interface->hasPacketBeenSent(packet3) == false);

    delete packet3;
}

TEST(NetworkInterfaceMockTest, equals) {
    NetworkInterfaceMock sameInterface = NetworkInterfaceMock("wlan0");
    NetworkInterfaceMock otherInterface = NetworkInterfaceMock("wlan1");

    CHECK(interface->equals(interface));
    CHECK(interface->equals(&sameInterface));
    CHECK(sameInterface.equals(interface));

    CHECK(interface->equals(&otherInterface) == false);
    CHECK(sameInterface.equals(&otherInterface) == false);
}

TEST(NetworkInterfaceMockTest, getNumberOfSentPackets) {
    Packet* packet1 = new PacketMock("Source", "Destination", 1);
    Packet* packet2 = new PacketMock("Source", "Destination", 2);
    Packet* packet3 = new PacketMock("Source", "Destination", 2);
    AddressPtr address1 (new AddressMock("A"));
    AddressPtr address2 (new AddressMock("B"));

    CHECK_EQUAL(0, interface->getNumberOfSentPackets());

    interface->send(packet1, address1);
    CHECK_EQUAL(1, interface->getNumberOfSentPackets());

    interface->send(packet2, address2);
    CHECK_EQUAL(2, interface->getNumberOfSentPackets());

    interface->send(packet3, address1);
    CHECK_EQUAL(3, interface->getNumberOfSentPackets());
}

TEST(NetworkInterfaceMockTest, getLocalAddress) {
    AddressPtr expectedLocalAddress (new AddressMock("DEFAULT"));
    AddressPtr defaultLocalAddress = interface->getLocalAddress();
    CHECK(defaultLocalAddress->equals(expectedLocalAddress));

    delete interface;
    interface = new NetworkInterfaceMock("wlan0", "192.168.0.1");
    expectedLocalAddress.reset(new AddressMock("192.168.0.1"));
    defaultLocalAddress = interface->getLocalAddress();
    CHECK(defaultLocalAddress->equals(expectedLocalAddress));
}

TEST(NetworkInterfaceMockTest, isBroadCastAddress) {
    AddressPtr someAddress1 (new AddressMock("192.168.0.1"));
    AddressPtr someAddress2 (new AddressMock("Foo"));
    AddressPtr broadcaseAddress (new AddressMock("BROADCAST"));

    CHECK_FALSE(interface->isBroadcastAddress(someAddress1));
    CHECK_FALSE(interface->isBroadcastAddress(someAddress2));
    CHECK_TRUE(interface->isBroadcastAddress(broadcaseAddress));
}

/**
 * This test just checks that there is no memory leak when sending packets.
 */
TEST(NetworkInterfaceMockTest, sendWithoutMemoryLeak) {
    Packet* packet = new PacketMock();
    AddressPtr recipient (new AddressMock());
    interface->send(packet, recipient);
}
