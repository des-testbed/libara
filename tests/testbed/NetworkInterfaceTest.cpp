/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "testbed/NetworkInterface.h"
#include "testAPI/mocks/ARAClientMock.h"


TESTBED_NAMESPACE_BEGIN

TEST_GROUP(NetworkInterfaceTest) {
    ARAClientMock* client;
    NetworkInterface* interface;

    void setup() {
        client = new ARAClientMock();
        interface = new NetworkInterface("eth0", client, client->getPacketFactory(), 400);
    }

    void teardown() {
        delete client;
        delete interface;
    }
};

TEST(NetworkInterfaceTest, notEquals) {
    NetworkInterface* tapInterface = new NetworkInterface("tap1", client, client->getPacketFactory(), 400);
    CHECK_FALSE(interface->equals(tapInterface));
}

TEST(NetworkInterfaceTest, equals) {
    NetworkInterface* ethInterface = new NetworkInterface("eth0", client, client->getPacketFactory(), 400);
    CHECK(interface->equals(ethInterface));
}

//The below is incorporated from ReliableNetworkInterfaceTest, interface is an object of type testbed/NetworkInterface

TEST(NetworkInterfaceTest, interfaceStoresUnacknowledgedPackets) {
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("foo"));

    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
    interface->send(packet, recipient);

    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());

    std::deque<const Packet*> unacknowledgedPackets = interface->getUnacknowledgedPackets();
    CHECK_EQUAL(packet, unacknowledgedPackets.front());
}

TEST(NetworkInterfaceTest, unacknowledgedPacketsAreDeletedInDestructor) {
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("foo"));

    interface->send(packet, recipient);

    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());
}

TEST(NetworkInterfaceTest, doNotWaitForAcknowledgmentOfBroadcastedPackets) {
    Packet* packet = new PacketMock();

    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
    interface->broadcast(packet);

    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
}

TEST(NetworkInterfaceTest, acknowledgeDataPacket) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, sequenceNr, 1);

    interface->receive(packet);

    BYTES_EQUAL(1, interface->getNumberOfSentPackets());

    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    CHECK(recipientOfSentPacket->equals(sender));
    CHECK_EQUAL(PacketType::ACK, sentPacket->getType());
    CHECK_EQUAL(sequenceNr, sentPacket->getSequenceNumber());
    CHECK(sentPacket->getSource()->equals(source));
}

TEST(NetworkInterfaceTest, doNotAcknowledgeAckPackets) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::ACK, sequenceNr, 1);

    interface->receive(packet);

    BYTES_EQUAL(0, interface->getNumberOfSentPackets());
}

TEST(NetworkInterfaceTest, doNotAcknowledgeAntPackets) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::FANT, sequenceNr, 1);

    CHECK(packet->isAntPacket());

    interface->receive(packet);

    BYTES_EQUAL(0, interface->getNumberOfSentPackets());
}

TEST(NetworkInterfaceTest, receivedPacketsAreDeliveredToTheClient) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, sequenceNr, 1);

    interface->receive(packet);

    BYTES_EQUAL(1, client->getNumberOfReceivedPackets());
    Pair<const Packet*, const NetworkInterface*>* receivedPacketInfo = client->getReceivedPackets().front();
    const Packet* receivedPacket = receivedPacketInfo->getLeft();
    const NetworkInterface* receivingInterface = receivedPacketInfo->getRight();
    CHECK(receivedPacket == packet);
    CHECK(receivingInterface == interface);
}

TEST(NetworkInterfaceTest, ackPacketsAreNotdeliveredToTheClient) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int sequenceNr = 123;
    Packet* packet = new Packet(source, destination, sender, PacketType::ACK, sequenceNr, 1);

    interface->receive(packet);

    BYTES_EQUAL(0, client->getNumberOfReceivedPackets());
}

TEST(NetworkInterfaceTest, acknowledgedPacketsAreRemovedFromListofUnacknowledgedPackets) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination1 (new AddressMock("destination1"));
    AddressPtr destination2 (new AddressMock("destination2"));
    AddressPtr sender (new AddressMock("sender"));
    Packet* packet1 = new Packet(source, destination1, sender, PacketType::DATA, 123, 1);
    Packet* packet2 = new Packet(source, destination1, sender, PacketType::DATA, 456, 1);

    interface->send(packet1, destination1);
    interface->send(packet2, destination2);

    BYTES_EQUAL(2, interface->getNrOfUnacknowledgedPackets());

    Packet* packetAck1 = new Packet(source, destination1, destination1, PacketType::ACK, 123, 1);
    interface->receive(packetAck1);

    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());
    std::deque<const Packet*> unacknowledgedPackets = interface->getUnacknowledgedPackets();
    CHECK_EQUAL(packet2, unacknowledgedPackets.front());

    Packet* packetAck2 = new Packet(source, destination2, destination2, PacketType::ACK, 456, 1);
    interface->receive(packetAck2);

    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
}


TEST(NetworkInterfaceTest, unacknowledgedPacketsAreSentAgain) {
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

    interface->send(packet, originalRecipient);

    BYTES_EQUAL(1, sentPackets->size());

    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* ackTimer = clock->getLastTimer();

    CHECK(ackTimer->isRunning());

    ackTimer->expire();

    CHECK(ackTimer->isRunning());

    BYTES_EQUAL(2, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->back();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    CHECK(recipientOfSentPacket->equals(originalRecipient));
    CHECK_PACKET(sentPacket, type, seqNr, ttl, source, sender, destination, payload);
}

TEST(NetworkInterfaceTest, routeFailuresAreReportedToARAClient) {
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

    interface->send(packet, originalRecipient);

    BYTES_EQUAL(1, sentPackets->size());

    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* ackTimer = clock->getLastTimer();

    ackTimer->expire();
    ackTimer->expire();
    ackTimer->expire();

    BYTES_EQUAL(1+3, sentPackets->size());
    for (int i = 1; i <= 3; i++) {
        Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->at(i);
        const Packet* sentPacket = sentPacketInfo->getLeft();
        AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

        CHECK(recipientOfSentPacket->equals(originalRecipient));
        CHECK_PACKET(sentPacket, type, seqNr, ttl, source, sender, destination, payload);
    }

    ackTimer->expire();

    BYTES_EQUAL(1, client->getNumberOfRouteFailures());
    ARAClientMock::PacketInfo routeFailurePacketInfo = client->getRouteFailurePackets().front();
    CHECK_PACKET(routeFailurePacketInfo.packet, type, seqNr, ttl, source, sender, destination, payload);
    CHECK(routeFailurePacketInfo.nextHop == originalRecipient);
    CHECK(routeFailurePacketInfo.interface == interface);
}

TEST(NetworkInterfaceTest, packetAcknowledgmentStopsTimer) {
    Packet* originalPacket = new PacketMock();
    AddressPtr originalRecipient = AddressPtr(new AddressMock("recipient"));

    interface->send(originalPacket, originalRecipient);

    BYTES_EQUAL(1, interface->getNrOfUnacknowledgedPackets());

    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* ackTimer = clock->getLastTimer();

    CHECK(ackTimer->isRunning());
    BYTES_EQUAL(1, interface->getNrOfRunningTimers());

    Packet* acknowledgment = client->getPacketFactory()->makeAcknowledgmentPacket(originalPacket, originalRecipient);
    interface->receive(acknowledgment);

    BYTES_EQUAL(0, interface->getNrOfRunningTimers());
}

TEST(NetworkInterfaceTest, doNotWaitForAcknowledgmentOfAckPAckets) {
    Packet* packet = new PacketMock();
    AddressPtr recipient = AddressPtr(new AddressMock("recipient"));

    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());

    interface->receive(packet);

    BYTES_EQUAL(0, interface->getNrOfUnacknowledgedPackets());
}

TESTBED_NAMESPACE_END
