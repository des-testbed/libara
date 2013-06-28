/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/AddressMock.h"
#include "EARAPacket.h"
#include "PacketType.h"

#include <cstring>
#include <climits>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EARAPacketTest) {};

TEST(EARAPacketTest, create) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    char type = PacketType::FANT;
    unsigned int seqNr = 1;
    int ttl = 15;
    const char* payload = "Hello World!";
    int payloadSize = strlen(payload)+1;

    EARAPacket packet = EARAPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);

    CHECK(packet.getSource()->equals(source));
    CHECK(packet.getDestination()->equals(destination));
    CHECK(packet.getSender()->equals(sender));
    CHECK_EQUAL(type, packet.getType());
    CHECK_EQUAL(seqNr, packet.getSequenceNumber());
    LONGS_EQUAL(ttl, packet.getTTL());
    LONGS_EQUAL(payloadSize, packet.getPayloadLength());
    STRCMP_EQUAL(payload, packet.getPayload());

    // most importantly this packet has some energy values
    LONGS_EQUAL(0, packet.getTotalEnergyValue());
    LONGS_EQUAL(UINT_MAX, packet.getMinimumEnergyValue());
}

TEST(EARAPacketTest, addEnergyValue) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));

    EARAPacket packet = EARAPacket(source, destination, source, PacketType::DATA, 123, 10);

    // start the test
    int addedEnergyValue = 666;
    LONGS_EQUAL(0, packet.getTotalEnergyValue());
    LONGS_EQUAL(UINT_MAX, packet.getMinimumEnergyValue());

    packet.addEnergyValue(addedEnergyValue);
    LONGS_EQUAL(addedEnergyValue, packet.getTotalEnergyValue());
    LONGS_EQUAL(addedEnergyValue, packet.getMinimumEnergyValue());

    packet.addEnergyValue(addedEnergyValue);
    LONGS_EQUAL(2*addedEnergyValue, packet.getTotalEnergyValue());
    LONGS_EQUAL(addedEnergyValue, packet.getMinimumEnergyValue());

    int biggerEnergyValue = 1000;
    packet.addEnergyValue(biggerEnergyValue);
    LONGS_EQUAL(2*addedEnergyValue + biggerEnergyValue, packet.getTotalEnergyValue());
    LONGS_EQUAL(addedEnergyValue, packet.getMinimumEnergyValue());

    int superLowEnergyValue = 1;
    packet.addEnergyValue(superLowEnergyValue);
    LONGS_EQUAL(2*addedEnergyValue + biggerEnergyValue + superLowEnergyValue, packet.getTotalEnergyValue());
    LONGS_EQUAL(superLowEnergyValue, packet.getMinimumEnergyValue());
}

/**
 * We check that the energy value is not used when comparing packets
 */
TEST(EARAPacketTest, equals) {
   unsigned int dataPacket = PacketType::DATA;
   const char* payload = "Hello World";
   int ttl = 15;

   AddressPtr source1 (new AddressMock("Source1"));
   AddressPtr source2 (new AddressMock("Source2"));
   AddressPtr destination (new AddressMock("Destination"));
   AddressPtr sender (new AddressMock("Sender"));

   EARAPacket packet = EARAPacket(source1, destination, sender, dataPacket, 1, ttl, payload, strlen(payload));
   EARAPacket samePacket = EARAPacket(source1, destination, sender, dataPacket, 1, ttl, payload, strlen(payload));

   CHECK(packet.equals(&samePacket));

   packet.addEnergyValue(123);
   samePacket.addEnergyValue(123);
   CHECK(packet.equals(&samePacket));

   // check if packets are still considered to be equal even if their energy values differ
   packet.addEnergyValue(1000);
   CHECK(packet.equals(&samePacket));
   CHECK(packet.getTotalEnergyValue() != samePacket.getTotalEnergyValue());
}

TEST(EARAPacketTest, overflow) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));

    EARAPacket packet = EARAPacket(source, destination, source, PacketType::DATA, 123, 10);
    LONGS_EQUAL(0, packet.getTotalEnergyValue());

    packet.addEnergyValue(UINT_MAX);
    LONGS_EQUAL(UINT_MAX, packet.getTotalEnergyValue());

    // if we add more than that it stays at the maximum
    packet.addEnergyValue(100);
    LONGS_EQUAL(UINT_MAX, packet.getTotalEnergyValue());
}
