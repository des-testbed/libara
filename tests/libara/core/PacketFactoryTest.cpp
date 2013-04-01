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
   const char* payload = "Hello World";

   Packet packet = Packet(source, destination, sender, type, seqNr, payload, strlen(payload), 123);
   unsigned int newSequenceNumber = 242342;
   Packet* fant = factory->makeFANT(&packet, newSequenceNumber);

   CHECK(fant->getSource()->equals(source));
   CHECK(fant->getDestination()->equals(destination));
   // The sender of a FANT will be determined when it is actually send by the ARA client
   CHECK_EQUAL(PacketType::FANT, fant->getType());
   LONGS_EQUAL(newSequenceNumber, fant->getSequenceNumber());
   LONGS_EQUAL(0, fant->getPayloadLength());
   LONGS_EQUAL(0, fant->getHopCount());

   delete fant;
}

TEST(PacketFactoryTest, makeClone) {
   AddressPtr source (new AddressMock("source"));
   AddressPtr destination (new AddressMock("destination"));
   AddressPtr sender (new AddressMock("sender"));
   char type = PacketType::DATA;
   unsigned int seqNr = 3;
   const char* payload = "Hello World";
   unsigned int payloadSize = strlen(payload)+1; // don't forget the \0 byte for string termination
   unsigned int hopCount = 123;

   Packet packet = Packet(source, destination, sender, type, seqNr, payload, payloadSize, hopCount);
   Packet* clone = factory->makeClone(&packet);

   CHECK(clone->getSource()->equals(source));
   CHECK(clone->getDestination()->equals(destination));
   CHECK(clone->getSender()->equals(sender));
   CHECK_EQUAL(type, clone->getType());
   CHECK_EQUAL(seqNr, clone->getSequenceNumber());
   LONGS_EQUAL(payloadSize, clone->getPayloadLength());
   STRCMP_EQUAL(payload, clone->getPayload());
   CHECK_EQUAL(hopCount, clone->getHopCount());
   CHECK(packet.equals(clone));

   delete clone;
}

TEST(PacketFactoryTest, makeBANT) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::FANT;
    int seqNr = 3;
    unsigned int hopCount = 123;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, seqNr, hopCount);
    unsigned int newSequenceNumber = 12345;
    Packet* bant = factory->makeBANT(&packet, newSequenceNumber);

    CHECK(bant->getSource()->equals(originalDestination));
    CHECK(bant->getDestination()->equals(originalSource));
    // The sender of the BANT will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::BANT, bant->getType());
    LONGS_EQUAL(newSequenceNumber, bant->getSequenceNumber());
    LONGS_EQUAL(0, bant->getPayloadLength());
    LONGS_EQUAL(0, bant->getHopCount());

    delete bant;
}

TEST(PacketFactoryTest, makeDulicateWarningPacket) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 3;
    unsigned int originalHopCount = 123;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalHopCount);
    Packet* duplicateWarning = factory->makeDulicateWarningPacket(&packet);

    CHECK(duplicateWarning->getSource()->equals(originalSource));
    CHECK(duplicateWarning->getDestination()->equals(originalDestination));
    // The sender of the packet will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::DUPLICATE_ERROR, duplicateWarning->getType());
    CHECK_EQUAL(originalseqenceNumber, duplicateWarning->getSequenceNumber());
    CHECK_EQUAL(0, duplicateWarning->getPayloadLength());
    CHECK_EQUAL(1, duplicateWarning->getHopCount());

    delete duplicateWarning;
}

TEST(PacketFactoryTest, makeAcknowledgmentPacket) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 123;
    unsigned int originalHopCount = 3;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalHopCount);
    Packet* ackPacket = factory->makeAcknowledgmentPacket(&packet);

    CHECK(ackPacket->getSource()->equals(originalSource));
    CHECK(ackPacket->getDestination()->equals(originalDestination));
    // The sender of the packet will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::ACK, ackPacket->getType());
    CHECK_EQUAL(originalseqenceNumber, ackPacket->getSequenceNumber());
    CHECK_EQUAL(0, ackPacket->getPayloadLength());
    CHECK_EQUAL(1, ackPacket->getHopCount());

    delete ackPacket;
}

TEST(PacketFactoryTest, makeRouteFailurePacket) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 123;
    unsigned int originalHopCount = 3;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalHopCount);
    Packet* routeFailurePacket = factory->makeRouteFailurePacket(&packet);

    CHECK(routeFailurePacket->getSource()->equals(originalSource));
    CHECK(routeFailurePacket->getDestination()->equals(originalDestination));
    // The sender of the packet will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::ROUTE_FAILURE, routeFailurePacket->getType());
    CHECK_EQUAL(originalseqenceNumber, routeFailurePacket->getSequenceNumber());
    CHECK_EQUAL(0, routeFailurePacket->getPayloadLength());
    CHECK_EQUAL(1, routeFailurePacket->getHopCount());

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
    CHECK_EQUAL(1, energyPacket->getHopCount());

    const char* payload = energyPacket->getPayload();
    BYTES_EQUAL(energyLevel, payload[0]);

    delete energyPacket;
}
