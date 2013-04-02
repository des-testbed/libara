/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include <cstring>
#include "testAPI/mocks/AddressMock.h"
#include "Packet.h"
#include "PacketType.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(PacketTest) {};

TEST(PacketTest, create) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));    // This is the node from which the packet has actually been received
    char type = PacketType::FANT;
    unsigned int seqNr = 1;

    Packet packet = Packet(source, destination, sender, type, seqNr);

    CHECK(packet.getSource()->equals(source));
    CHECK(packet.getDestination()->equals(destination));
    CHECK(packet.getSender()->equals(sender));
    CHECK_EQUAL(type, packet.getType());
    CHECK_EQUAL(seqNr, packet.getSequenceNumber());
    LONGS_EQUAL(0, packet.getHopCount());

    CHECK_EQUAL(0, packet.getPayloadLength());
    CHECK(packet.getPayload() == false);
}

TEST(PacketTest, createWithPayload) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    char type = PacketType::DATA;
    unsigned int seqNr = 2;
    const char* payload = "Hello World!";
    int payloadSize = strlen(payload)+1;

    Packet packet = Packet(source, destination, sender, type, seqNr, payload, payloadSize);

    CHECK(packet.getSource()->equals(source));
    CHECK(packet.getDestination()->equals(destination));
    CHECK(packet.getSender()->equals(sender));
    CHECK_EQUAL(type, packet.getType());
    CHECK_EQUAL(seqNr, packet.getSequenceNumber());
    LONGS_EQUAL(0, packet.getHopCount());

    LONGS_EQUAL(payloadSize, packet.getPayloadLength());
    STRCMP_EQUAL(payload, packet.getPayload());
}

TEST(PacketTest, createWithPayloadAndHopCount) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    char type = PacketType::DATA;
    unsigned int seqNr = 3;
    const char* payload = "Hello World";
    int payloadSize = strlen(payload)+1;
    unsigned int hopCount = 123;

    Packet packet = Packet(source, destination, sender, type, seqNr, payload, payloadSize, hopCount);

    CHECK(packet.getSource()->equals(source));
    CHECK(packet.getDestination()->equals(destination));
    CHECK(packet.getSender()->equals(sender));
    CHECK_EQUAL(type, packet.getType());
    CHECK_EQUAL(seqNr, packet.getSequenceNumber());
    LONGS_EQUAL(payloadSize, packet.getPayloadLength());
    STRCMP_EQUAL(payload, packet.getPayload());
    CHECK_EQUAL(hopCount, packet.getHopCount());
}

TEST(PacketTest, createWithoutPayloadButWithHopCount) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    char type = PacketType::FANT;
    unsigned int seqNr = 1;
    unsigned int hopCount = 7;

    Packet packet = Packet(source, destination, sender, type, seqNr, hopCount);

    CHECK(packet.getSource()->equals(source));
    CHECK(packet.getDestination()->equals(destination));
    CHECK(packet.getSender()->equals(sender));
    CHECK_EQUAL(type, packet.getType());
    CHECK_EQUAL(seqNr, packet.getSequenceNumber());
    CHECK_EQUAL(hopCount, packet.getHopCount());

    CHECK_EQUAL(0, packet.getPayloadLength());
    CHECK(packet.getPayload() == false);
}

TEST(PacketTest, setHopCount) {
   AddressPtr source (new AddressMock("source"));
   AddressPtr destination (new AddressMock("destination"));
   AddressPtr sender (new AddressMock("sender"));
   unsigned int type = PacketType::DATA;
   int seqNr = 3;
   const char* payload = "Hello World";
   unsigned int hopCount = 123;

   Packet packet = Packet(source, destination, sender, type, seqNr, payload, strlen(payload), hopCount);
   CHECK_EQUAL(hopCount, packet.getHopCount());

   unsigned int newHopCount = 124;
   packet.setHopCount(newHopCount);
   CHECK_EQUAL(newHopCount, packet.getHopCount());
}

TEST(PacketTest, equals) {
   unsigned int dataPacket = PacketType::DATA;
   const char* payload = "Hello World";

   AddressPtr source1 (new AddressMock("Source1"));
   AddressPtr source2 (new AddressMock("Source2"));
   AddressPtr destination (new AddressMock("Destination"));
   AddressPtr sender (new AddressMock("Sender"));

   Packet packet            = Packet(source1, destination, sender, dataPacket, 1, payload, strlen(payload));
   Packet samePacket        = Packet(source1, destination, sender, dataPacket, 1, payload, strlen(payload));
   Packet nextSeqPacket     = Packet(source2, destination, sender, dataPacket, 2, payload, strlen(payload));
   Packet otherSourcePacket = Packet(source2, destination, sender, dataPacket, 1, payload, strlen(payload));
   Packet otherPacket       = Packet(source2, destination, sender, dataPacket, 3, payload, strlen(payload));

   CHECK(packet.equals(&packet));
   CHECK(packet.equals(&samePacket));
   CHECK(packet.equals(&nextSeqPacket) == false);
   CHECK(packet.equals(&otherSourcePacket) == false);
   CHECK(packet.equals(&otherPacket) == false);
}

TEST(PacketTest, calculatePayloadLength) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    char type = PacketType::DATA;
    unsigned int seqNr = 1;
    const char* payload = "Hello World";

    Packet packet = Packet(source, destination, sender, type, seqNr, payload);
    LONGS_EQUAL(strlen(payload)+1, packet.getPayloadLength())
}

TEST(PacketTest, setSender) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender1 (new AddressMock("originalSender"));
    AddressPtr sender2 (new AddressMock("originalSender"));
    char type = PacketType::DATA;
    unsigned int seqNr = 1;
    const char* payload = "Hello World";

    Packet packet = Packet(source, destination, sender1, type, seqNr, payload);
    CHECK(packet.getSender()->equals(sender1));

    packet.setSender(source);   // will delete sender1
    CHECK(packet.getSender()->equals(source));

    // now we test what happens if the original sender has been the source
    packet.setSender(sender2);  // should NOT delete source
    CHECK(packet.getSender()->equals(sender2));
    CHECK(packet.getSource()->equals(source));
}

TEST(PacketTest, increaseHopCount) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("originalSender"));
    char type = PacketType::DATA;
    unsigned int seqNr = 1;
    const char* payload = "Hello World";
    unsigned int hopCount = 5;

    Packet packet = Packet(source, destination, sender, type, seqNr, payload, 11, hopCount);
    LONGS_EQUAL(hopCount, packet.getHopCount());

    packet.increaseHopCount();
    LONGS_EQUAL(hopCount+1, packet.getHopCount());

    packet.increaseHopCount();
    LONGS_EQUAL(hopCount+2, packet.getHopCount());
}

TEST(PacketTest, getHashValue) {
    AddressPtr address1 (new AddressMock("A"));
    AddressPtr address2 (new AddressMock("B"));
    Packet packet1 = Packet(address1, address2, PacketType::DATA, 1);
    Packet packet2 = Packet(address1, address2, PacketType::DATA, 2);
    Packet packet3 = Packet(address1, address2, PacketType::DATA, 3);
    Packet packet4 = Packet(address2, address1, PacketType::DATA, 1);
    Packet packet5 = Packet(address2, address1, PacketType::DATA, 1);

    CHECK(packet1.getHashValue() != packet2.getHashValue()); // same source - different seqNr
    CHECK(packet2.getHashValue() != packet3.getHashValue()); // same source - different seqNr
    CHECK(packet1.getHashValue() != packet4.getHashValue()); // different source - same seqNr
    CHECK(packet4.getHashValue() == packet5.getHashValue()); // same source - same seqNr
}

TEST(PacketTest, getAddressString) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 3;
    unsigned int originalHopCount = 123;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalHopCount);
    STRCMP_EQUAL("source", packet.getSourceString());
    STRCMP_EQUAL("sender", packet.getSenderString());
    STRCMP_EQUAL("destination", packet.getDestinationString());
}

TEST(PacketTest, decreaseHopCount) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("originalSender"));
    char type = PacketType::DATA;
    unsigned int seqNr = 1;
    const char* payload = "Hello World";
    unsigned int hopCount = 5;

    Packet packet = Packet(source, destination, sender, type, seqNr, payload, 11, hopCount);
    LONGS_EQUAL(hopCount, packet.getHopCount());

    packet.decreaseHopCount();
    LONGS_EQUAL(hopCount-1, packet.getHopCount());

    packet.decreaseHopCount();
    LONGS_EQUAL(hopCount-2, packet.getHopCount());
}
