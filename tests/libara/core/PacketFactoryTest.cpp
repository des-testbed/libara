/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "PacketFactory.h"
#include "testAPI/mocks/AddressMock.h"
#include "Packet.h"
#include "PacketType.h"

#include <cstring>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(PacketFactoryTest) {
    PacketFactory* factory;

    void setup() {
        factory = new PacketFactory();
    }

    void teardown() {
        delete factory;
    }
};

TEST(PacketFactoryTest, makeFANT) {
   AddressPtr source (new AddressMock("source"));
   AddressPtr destination (new AddressMock("destination"));
   AddressPtr sender (new AddressMock("sender"));
   unsigned int type = PacketType::DATA;
   int seqNr = 3;
   int maximumHopCount = 20;
   int ttl = 15;
   const char* payload = "Hello World";

   Packet packet = Packet(source, destination, sender, type, seqNr, ttl, payload, strlen(payload));
   unsigned int newSequenceNumber = 242342;
   Packet* fant = factory->makeFANT(&packet, newSequenceNumber, maximumHopCount);

   CHECK(fant->getSource()->equals(source));
   CHECK(fant->getDestination()->equals(destination));
   // The sender of a FANT will be determined when it is actually send by the ARA client
   CHECK_EQUAL(PacketType::FANT, fant->getType());
   LONGS_EQUAL(newSequenceNumber, fant->getSequenceNumber());
   LONGS_EQUAL(maximumHopCount, fant->getTTL());
   LONGS_EQUAL(0, fant->getPayloadLength());

   delete fant;
}

TEST(PacketFactoryTest, makeClone) {
   AddressPtr source (new AddressMock("source"));
   AddressPtr destination (new AddressMock("destination"));
   AddressPtr sender (new AddressMock("sender"));
   char type = PacketType::DATA;
   unsigned int seqNr = 3;
   int ttl = 15;
   const char* payload = "Hello World";
   unsigned int payloadSize = strlen(payload)+1; // don't forget the \0 byte for string termination

   Packet packet = Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
   Packet* clone = factory->makeClone(&packet);

   CHECK(clone->getSource()->equals(source));
   CHECK(clone->getDestination()->equals(destination));
   CHECK(clone->getSender()->equals(sender));
   CHECK_EQUAL(type, clone->getType());
   CHECK_EQUAL(seqNr, clone->getSequenceNumber());
   LONGS_EQUAL(payloadSize, clone->getPayloadLength());
   STRCMP_EQUAL(payload, clone->getPayload());
   CHECK_EQUAL(ttl, clone->getTTL());
   CHECK(packet.equals(clone));

   delete clone;
}

TEST(PacketFactoryTest, makeBANT) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::FANT;
    int seqNr = 3;
    int maximumHopCount = 20;
    int ttl = 15;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, seqNr, ttl);
    unsigned int newSequenceNumber = 12345;
    Packet* bant = factory->makeBANT(&packet, newSequenceNumber, maximumHopCount);

    CHECK(bant->getSource()->equals(originalDestination));
    CHECK(bant->getDestination()->equals(originalSource));
    // The sender of the BANT will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::BANT, bant->getType());
    LONGS_EQUAL(newSequenceNumber, bant->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, bant->getTTL());
    LONGS_EQUAL(0, bant->getPayloadLength());

    delete bant;
}

TEST(PacketFactoryTest, makeDulicateErrorPacket) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    AddressPtr senderOfDuplicateWarning (new AddressMock("foo"));
    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 3;
    unsigned int originalTTL = 20;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalTTL);
    unsigned int newSequenceNumber = 12345;
    Packet* duplicateWarning = factory->makeDulicateWarningPacket(&packet, senderOfDuplicateWarning, newSequenceNumber);

    CHECK(duplicateWarning->getSource()->equals(senderOfDuplicateWarning));
    CHECK(duplicateWarning->getDestination()->equals(originalDestination));
    CHECK(duplicateWarning->getSender()->equals(senderOfDuplicateWarning));
    CHECK_EQUAL(PacketType::DUPLICATE_ERROR, duplicateWarning->getType());
    CHECK_EQUAL(newSequenceNumber, duplicateWarning->getSequenceNumber());
    CHECK_EQUAL(0, duplicateWarning->getPayloadLength());
    CHECK_EQUAL(1, duplicateWarning->getTTL());

    delete duplicateWarning;
}

TEST(PacketFactoryTest, makeAcknowledgmentPacket) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 123;
    unsigned int originalTTL = 10;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalTTL);
    Packet* ackPacket = factory->makeAcknowledgmentPacket(&packet);

    CHECK(ackPacket->getSource()->equals(originalSource));
    CHECK(ackPacket->getDestination()->equals(originalDestination));
    // The sender of the packet will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::ACK, ackPacket->getType());
    CHECK_EQUAL(originalseqenceNumber, ackPacket->getSequenceNumber());
    CHECK_EQUAL(0, ackPacket->getPayloadLength());
    CHECK_EQUAL(1, ackPacket->getTTL());

    delete ackPacket;
}

TEST(PacketFactoryTest, makeRouteFailurePacket) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 123;
    unsigned int originalTTL = 30;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalTTL);
    Packet* routeFailurePacket = factory->makeRouteFailurePacket(&packet);

    CHECK(routeFailurePacket->getSource()->equals(originalSource));
    CHECK(routeFailurePacket->getDestination()->equals(originalDestination));
    // The sender of the packet will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::ROUTE_FAILURE, routeFailurePacket->getType());
    CHECK_EQUAL(originalseqenceNumber, routeFailurePacket->getSequenceNumber());
    CHECK_EQUAL(0, routeFailurePacket->getPayloadLength());
    CHECK_EQUAL(1, routeFailurePacket->getTTL());

    delete routeFailurePacket;
}

TEST(PacketFactoryTest, makeEnergyDisseminationPacket) {
    AddressPtr originalSource (new AddressMock("source"));
    unsigned int seqNr = 123;
    unsigned char energyLevel = 255;
    Packet* energyPacket = factory->makeEnergyDisseminationPacket(originalSource, seqNr, energyLevel);

    CHECK(energyPacket->getSource()->equals(originalSource));
    CHECK(energyPacket->getSender()->equals(originalSource));
    // we do not need to check the destination field because it will be set to the broadcast address by the NIC

    CHECK_EQUAL(PacketType::ENERGY_INFO, energyPacket->getType());
    CHECK_EQUAL(seqNr, energyPacket->getSequenceNumber());
    CHECK_EQUAL(1, energyPacket->getPayloadLength());
    CHECK_EQUAL(1, energyPacket->getTTL());

    const char* payload = energyPacket->getPayload();
    BYTES_EQUAL(energyLevel, payload[0]);

    delete energyPacket;
}
