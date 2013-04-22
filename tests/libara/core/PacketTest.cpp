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
    int ttl = 15;

    Packet packet = Packet(source, destination, sender, type, seqNr, ttl);

    CHECK(packet.getSource()->equals(source));
    CHECK(packet.getDestination()->equals(destination));
    CHECK(packet.getSender()->equals(sender));
    CHECK_EQUAL(type, packet.getType());
    CHECK_EQUAL(seqNr, packet.getSequenceNumber());
    LONGS_EQUAL(ttl, packet.getTTL());

    CHECK_EQUAL(0, packet.getPayloadLength());
    CHECK(packet.getPayload() == false);
}

TEST(PacketTest, createWithPayload) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    char type = PacketType::DATA;
    unsigned int seqNr = 2;
    int ttl = 15;
    const char* payload = "Hello World!";
    int payloadSize = strlen(payload)+1;

    Packet packet = Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);

    CHECK(packet.getSource()->equals(source));
    CHECK(packet.getDestination()->equals(destination));
    CHECK(packet.getSender()->equals(sender));
    CHECK_EQUAL(type, packet.getType());
    CHECK_EQUAL(seqNr, packet.getSequenceNumber());
    LONGS_EQUAL(ttl, packet.getTTL());

    LONGS_EQUAL(payloadSize, packet.getPayloadLength());
    STRCMP_EQUAL(payload, packet.getPayload());
}

TEST(PacketTest, equals) {
   unsigned int dataPacket = PacketType::DATA;
   const char* payload = "Hello World";
   int ttl = 15;

   AddressPtr source1 (new AddressMock("Source1"));
   AddressPtr source2 (new AddressMock("Source2"));
   AddressPtr destination (new AddressMock("Destination"));
   AddressPtr sender (new AddressMock("Sender"));

   Packet packet            = Packet(source1, destination, sender, dataPacket, 1, ttl, payload, strlen(payload));
   Packet samePacket        = Packet(source1, destination, sender, dataPacket, 1, ttl, payload, strlen(payload));
   Packet nextSeqPacket     = Packet(source2, destination, sender, dataPacket, 2, ttl, payload, strlen(payload));
   Packet otherSourcePacket = Packet(source2, destination, sender, dataPacket, 1, ttl, payload, strlen(payload));
   Packet otherPacket       = Packet(source2, destination, sender, dataPacket, 3, ttl, payload, strlen(payload));

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
    int ttl = 15;
    const char* payload = "Hello World";

    Packet packet = Packet(source, destination, sender, type, seqNr, ttl, payload);
    LONGS_EQUAL(strlen(payload)+1, packet.getPayloadLength())
}

TEST(PacketTest, setSender) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender1 (new AddressMock("A"));
    AddressPtr sender2 (new AddressMock("B"));
    char type = PacketType::DATA;
    unsigned int seqNr = 1;
    int ttl = 15;
    const char* payload = "Hello World";

    Packet packet = Packet(source, destination, sender1, type, seqNr, ttl, payload);
    CHECK(packet.getSender()->equals(sender1));

    packet.setSender(source);   // will delete sender1
    CHECK(packet.getSender()->equals(source));

    // now we test what happens if the original sender has been the source
    packet.setSender(sender2);  // should NOT delete source
    CHECK(packet.getSender()->equals(sender2));
    CHECK(packet.getSource()->equals(source));
}

TEST(PacketTest, getHashValue) {
    AddressPtr address1 (new AddressMock("A"));
    AddressPtr address2 (new AddressMock("B"));
    AddressPtr sender (new AddressMock("B"));
    Packet packet1 = Packet(address1, address2, sender, PacketType::DATA, 1, 15);
    Packet packet2 = Packet(address1, address2, sender, PacketType::DATA, 2, 8);
    Packet packet3 = Packet(address1, address2, sender, PacketType::DATA, 3, 10);
    Packet packet4 = Packet(address2, address1, sender, PacketType::DATA, 1, 7);
    Packet packet5 = Packet(address2, address1, sender, PacketType::DATA, 1, 2);

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
    unsigned int seqenceNumber = 3;
    int ttl = 20;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, seqenceNumber, ttl);
    STRCMP_EQUAL("source", packet.getSourceString());
    STRCMP_EQUAL("sender", packet.getSenderString());
    STRCMP_EQUAL("destination", packet.getDestinationString());
}

TEST(PacketTest, setPenultimateHop) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr penultimateHop1 (new AddressMock("A"));
    AddressPtr penultimateHop2 (new AddressMock("B"));
    char type = PacketType::DATA;
    unsigned int seqNr = 1;
    int ttl = 15;
    const char* payload = "Hello World";

    Packet packet = Packet(source, destination, sender, type, seqNr, ttl, payload);
    CHECK(packet.getPenultimateHop() == nullptr);

    packet.setPenultimateHop(penultimateHop1);
    CHECK(packet.getPenultimateHop()->equals(penultimateHop1));

    packet.setPenultimateHop(penultimateHop2);
    CHECK(packet.getPenultimateHop()->equals(penultimateHop2));
}
