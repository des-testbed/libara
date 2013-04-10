/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "TestMacros.h"
#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/time/ClockMock.h"
#include "Environment.h"

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(ReliableNetworkInterfaceTest) {
    ARAClientMock* client;
    NetworkInterfaceMock* interface;
    std::deque<Pair<const Packet*, std::shared_ptr<Address>>*>* sentPackets;

    void setup() {
        client = new ARAClientMock();
        interface = new NetworkInterfaceMock(client);
        sentPackets = interface->getSentPackets();
    }

    void teardown() {
        delete interface;
        delete client;
    }
};

TEST(ReliableNetworkInterfaceTest, interfaceStoresUnacknowledgedPackets) {
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("foo"));

    // start the test
    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
    interface->send(packet, recipient);

    // test if the interface is waiting for a packet acknowledgment
    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());

    std::deque<const Packet*> unacknowledgedPackets = interface->getUnacknowledgedPackets();
    CHECK_EQUAL(packet, unacknowledgedPackets.front());
}

TEST(ReliableNetworkInterfaceTest, unacknowledgedPacketsAreDeletedInDestructor) {
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("foo"));

    interface->send(packet, recipient);

    // test if the interface is waiting for a packet acknowledgment
    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());

    // the packet should be deleted in the interface destructor
}

TEST(ReliableNetworkInterfaceTest, doNotWaitForAcknowledgmentOfBroadcastedPackets) {
    Packet* packet = new PacketMock();

    // start the test
    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
    interface->broadcast(packet);

    // this should not change the nr of unacknowledged packets because we never wait for a broadcast
    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
}

TEST(ReliableNetworkInterfaceTest, acknowledgeDataPacket) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, sequenceNr, 1);

    interface->receive(packet);

    // the interface is required to acknowledge this data packet
    BYTES_EQUAL(1, interface->getNumberOfSentPackets());

    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    CHECK(recipientOfSentPacket->equals(sender));
    CHECK_EQUAL(PacketType::ACK, sentPacket->getType());
    CHECK_EQUAL(sequenceNr, sentPacket->getSequenceNumber());
    CHECK(sentPacket->getSource()->equals(source));
}

TEST(ReliableNetworkInterfaceTest, doNotAcknowledgeAckPackets) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::ACK, sequenceNr, 1);

    interface->receive(packet);

    // we should never acknowledge acknowledgment packets
    BYTES_EQUAL(0, interface->getNumberOfSentPackets());
}

TEST(ReliableNetworkInterfaceTest, doNotAcknowledgeAntPackets) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::FANT, sequenceNr, 1);

    // sanity check
    CHECK(packet->isAntPacket());

    interface->receive(packet);

    // we should never acknowledge ant packets
    BYTES_EQUAL(0, interface->getNumberOfSentPackets());
}

TEST(ReliableNetworkInterfaceTest, receivedPacketsAreDeliveredToTheClient) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, sequenceNr, 1);

    interface->receive(packet);

    // check if the packet has indeed been delivered to the client
    BYTES_EQUAL(1, client->getNumberOfReceivedPackets());
    Pair<const Packet*, const NetworkInterface*>* receivedPacketInfo = client->getReceivedPackets().front();
    const Packet* receivedPacket = receivedPacketInfo->getLeft();
    const NetworkInterface* receivingInterface = receivedPacketInfo->getRight();
    CHECK(receivedPacket == packet);
    CHECK(receivingInterface == interface);
}

TEST(ReliableNetworkInterfaceTest, ackPacketsAreNotdeliveredToTheClient) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::ACK, sequenceNr, 1);

    interface->receive(packet);

    // check that the ACK is not delivered to the client
    BYTES_EQUAL(0, client->getNumberOfReceivedPackets());
}

TEST(ReliableNetworkInterfaceTest, acknowledgedPacketsAreRemovedFromListofUnacknowledgedPackets) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination1 (new AddressMock("destination1"));
    AddressPtr destination2 (new AddressMock("destination2"));
    AddressPtr sender (new AddressMock("sender"));
    Packet* packet1 = new Packet(source, destination1, sender, PacketType::DATA, 123, 1);
    Packet* packet2 = new Packet(source, destination1, sender, PacketType::DATA, 456, 1);

    // start the test
    interface->send(packet1, destination1);
    interface->send(packet2, destination2);

    // test if the interface is now waiting for a packet acknowledgment
    BYTES_EQUAL(2, interface->getNrOfUnacknowledgedPackets());

    // emulate the packet acknowledgment for packet1
    Packet* packetAck1 = new Packet(source, destination1, destination1, PacketType::ACK, 123, 1);
    interface->receive(packetAck1);

    // packet must be deleted from the list of unacknowledged packets so only packet2 remains in there
    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());
    std::deque<const Packet*> unacknowledgedPackets = interface->getUnacknowledgedPackets();
    CHECK_EQUAL(packet2, unacknowledgedPackets.front());

    // emulate the packet acknowledgment for packet2
    Packet* packetAck2 = new Packet(source, destination2, destination2, PacketType::ACK, 456, 1);
    interface->receive(packetAck2);

    // the last packet is deleted from the list of unacknowledged packets
    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
}


TEST(ReliableNetworkInterfaceTest, unacknowledgedPacketsAreSentAgain) {
    // prepare the test
    int type = PacketType::DATA;
    unsigned int seqNr = 123;
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination (new AddressMock("destination"));
    unsigned int ttl = 15;
    const char* payload = "Hello World";
    int payloadSize = std::strlen(payload)+1;
    Packet* packet = new Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    AddressPtr originalRecipient = AddressPtr(new AddressMock("recipient"));

    // start the test
    interface->send(packet, originalRecipient);

    // make sure the packet has been sent once
    BYTES_EQUAL(1, sentPackets->size());

    // get the acknowledgment timer which is used by the interface
    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* ackTimer = clock->getLastTimer();

    CHECK(ackTimer->isRunning());

    // simulate that the timer has expired (timeout)
    ackTimer->expire();

    // the timer should have been restarted
    CHECK(ackTimer->isRunning());

    // the packet should have been retransmitted again
    BYTES_EQUAL(2, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->back();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    CHECK(recipientOfSentPacket->equals(originalRecipient));
    CHECK_PACKET(sentPacket, type, seqNr, ttl, source, sender, destination, payload);
}

TEST(ReliableNetworkInterfaceTest, routeFailuresAreReportedToARAClient) {
    // prepare the test
    int type = PacketType::DATA;
    unsigned int seqNr = 123;
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination (new AddressMock("destination"));
    unsigned int ttl = 10;
    const char* payload = "Hello World";
    int payloadSize = std::strlen(payload)+1;
    Packet* packet = new Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    AddressPtr originalRecipient = AddressPtr(new AddressMock("recipient"));
    interface->setMaxNrOfRetransmissions(3);

    // start the test
    interface->send(packet, originalRecipient);

    // make sure the packet has been sent once
    BYTES_EQUAL(1, sentPackets->size());

    // get the acknowledgment timer which is used by the interface
    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* ackTimer = clock->getLastTimer();

    // simulate that the timer does expire 3 times
    ackTimer->expire();
    ackTimer->expire();
    ackTimer->expire();

    // the packet should have been retransmitted again
    BYTES_EQUAL(1+3, sentPackets->size());
    for (int i = 1; i <= 3; i++) {
        Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->at(i);
        const Packet* sentPacket = sentPacketInfo->getLeft();
        AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

        CHECK(recipientOfSentPacket->equals(originalRecipient));
        CHECK_PACKET(sentPacket, type, seqNr, ttl, source, sender, destination, payload);
    }

    // now if we let the timer expire one more time the packet should be reported route failure to the client
    ackTimer->expire();

    BYTES_EQUAL(1, client->getNumberOfRouteFailures());
    ARAClientMock::PacketInfo routeFailurePacketInfo = client->getRouteFailurePackets().front();
    CHECK_PACKET(routeFailurePacketInfo.packet, type, seqNr, ttl, source, sender, destination, payload);
    CHECK(routeFailurePacketInfo.nextHop == originalRecipient);
    CHECK(routeFailurePacketInfo.interface == interface);
}

TEST(ReliableNetworkInterfaceTest, packetAcknowledgmentStopsTimer) {
    // prepare the test
    Packet* originalPacket = new PacketMock();
    AddressPtr originalRecipient = AddressPtr(new AddressMock("recipient"));

    // start the test
    interface->send(originalPacket, originalRecipient);

    // make sure the interface is waiting for an acknowledgment
    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());

    // get the acknowledgment timer which is used by the interface
    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* ackTimer = clock->getLastTimer();

    // the timer should have been started
    CHECK(ackTimer->isRunning());
    BYTES_EQUAL(1, interface->getNrOfRunningTimers());

    // let the interface receive the acknowledgment
    Packet* acknowledgment = Environment::getPacketFactory()->makeAcknowledgmentPacket(originalPacket);
    interface->receive(acknowledgment);

    BYTES_EQUAL(0, interface->getNrOfRunningTimers());
}

TEST(ReliableNetworkInterfaceTest, doNotWaitForAcknowledgmentOfAckPAckets) {
    // prepare the test
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("recipient"));

    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());

    // the interface is expected to send an acknowledgment
    interface->receive(packet);

    // check that the interface is still not awaiting any acknowledgment
    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
}
