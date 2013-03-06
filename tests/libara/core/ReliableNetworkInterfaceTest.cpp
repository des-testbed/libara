/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(ReliableNetworkInterfaceTest) {
    NetworkInterfaceMock* interface;

    void setup() {
        interface = new NetworkInterfaceMock();
    }

    void teardown() {
        delete interface;
    }
};

TEST(ReliableNetworkInterfaceTest, interfaceStoresUnacknowledgedPackets) {
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("foo"));

    // start the test
    CHECK_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
    interface->send(packet, recipient);

    // test if the interface is waiting for a packet acknowledgment
    CHECK_EQUAL(1, interface->getNrOfUnacknowledgedPackets());

    std::deque<const Packet*> unacknowledgedPackets = interface->getUnacknowledgedPackets();
    CHECK_EQUAL(packet, unacknowledgedPackets.front());
}

TEST(ReliableNetworkInterfaceTest, unacknowledgedPacketsAreDeletedInDestructor) {
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("foo"));

    interface->send(packet, recipient);

    // test if the interface is waiting for a packet acknowledgment
    CHECK_EQUAL(1, interface->getNrOfUnacknowledgedPackets());

    // the packet should be deleted in the interface destructor
}

TEST(ReliableNetworkInterfaceTest, doNotWaitForAcknowledgmentOfBroadcastedPackets) {
    Packet* packet = new PacketMock();

    // start the test
    CHECK_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
    interface->broadcast(packet);

    // this should not change the nr of unacknowledged packets because we never wait for a broadcast
    CHECK_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
}
