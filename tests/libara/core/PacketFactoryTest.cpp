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
    int maximumHopCount= 15;

    void setup() {
        factory = new PacketFactory(maximumHopCount);
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
   int ttl = 15;
   const char* payload = "Hello World";

   Packet packet = Packet(source, destination, sender, type, seqNr, ttl, payload, strlen(payload));
   unsigned int newSequenceNumber = 242342;
   Packet* fant = factory->makeFANT(source, destination, newSequenceNumber);

   CHECK(fant->getSource()->equals(source));
   CHECK(fant->getDestination()->equals(destination));
   CHECK(fant->getSender()->equals(source));
   CHECK(fant->getPreviousHop()->equals(source));
   CHECK(fant->getType() == PacketType::FANT);
   LONGS_EQUAL(newSequenceNumber, fant->getSequenceNumber());
   LONGS_EQUAL(maximumHopCount, fant->getTTL());
   LONGS_EQUAL(0, fant->getPayloadLength());

   delete fant;
}

TEST(PacketFactoryTest, makeClone) {
   AddressPtr source (new AddressMock("source"));
   AddressPtr destination (new AddressMock("destination"));
   AddressPtr sender (new AddressMock("sender"));
   AddressPtr prevHop (new AddressMock("penultimate"));
   char type = PacketType::DATA;
   unsigned int seqNr = 3;
   int ttl = 15;
   const char* payload = "Hello World";
   unsigned int payloadSize = strlen(payload)+1; // don't forget the \0 byte for string termination

   Packet packet = Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
   packet.setPreviousHop(prevHop);
   Packet* clone = factory->makeClone(&packet);

   CHECK(clone->getSource()->equals(source));
   CHECK(clone->getDestination()->equals(destination));
   CHECK(clone->getSender()->equals(sender));
   CHECK(clone->getPreviousHop() != nullptr);
   CHECK(clone->getPreviousHop()->equals(prevHop));
   CHECK(clone->getType() == type);
   LONGS_EQUAL(seqNr, clone->getSequenceNumber());
   LONGS_EQUAL(payloadSize, clone->getPayloadLength());
   STRCMP_EQUAL(payload, clone->getPayload());
   LONGS_EQUAL(ttl, clone->getTTL());
   CHECK(packet.equals(clone));

   delete clone;
}

TEST(PacketFactoryTest, makeBANT) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int type = PacketType::FANT;
    int seqNr = 3;
    int ttl = 15;

    Packet packet = Packet(originalSource, originalDestination, originalSender, type, seqNr, ttl);
    unsigned int newSequenceNumber = 12345;
    Packet* bant = factory->makeBANT(&packet, newSequenceNumber);

    CHECK(bant->getSource()->equals(originalDestination));
    CHECK(bant->getDestination()->equals(originalSource));
    CHECK(bant->getSender()->equals(originalDestination));
    CHECK(bant->getPreviousHop()->equals(originalDestination));
    CHECK(bant->getType() == PacketType::BANT);
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
    CHECK(duplicateWarning->getPreviousHop()->equals(senderOfDuplicateWarning));
    CHECK(duplicateWarning->getType() == PacketType::DUPLICATE_ERROR);
    LONGS_EQUAL(newSequenceNumber, duplicateWarning->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, duplicateWarning->getTTL());
    LONGS_EQUAL(0, duplicateWarning->getPayloadLength());

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
    Packet* ackPacket = factory->makeAcknowledgmentPacket(&packet, originalSender);

    CHECK(ackPacket->getSource()->equals(originalSource));
    CHECK(ackPacket->getDestination()->equals(originalDestination));
    CHECK(ackPacket->getSender()->equals(originalSender));
    CHECK(ackPacket->getPreviousHop()->equals(originalSender));
    CHECK(ackPacket->getType() == PacketType::ACK);
    LONGS_EQUAL(originalseqenceNumber, ackPacket->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, ackPacket->getTTL());
    LONGS_EQUAL(0, ackPacket->getPayloadLength());

    delete ackPacket;
}

TEST(PacketFactoryTest, makeRouteFailurePacket) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    unsigned int seqenceNumber = 123;

    Packet* routeFailurePacket = factory->makeRouteFailurePacket(source, destination, seqenceNumber);

    CHECK(routeFailurePacket->getSource()->equals(source));
    CHECK(routeFailurePacket->getDestination()->equals(destination));
    CHECK(routeFailurePacket->getSender()->equals(source));
    CHECK(routeFailurePacket->getPreviousHop()->equals(source));
    CHECK(routeFailurePacket->getType() == PacketType::ROUTE_FAILURE);
    LONGS_EQUAL(seqenceNumber, routeFailurePacket->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, routeFailurePacket->getTTL());
    LONGS_EQUAL(0, routeFailurePacket->getPayloadLength());

    delete routeFailurePacket;
}

TEST(PacketFactoryTest, getMaximumNrOfHops){
    BYTES_EQUAL(maximumHopCount, factory->getMaximumNrOfHops());
}

TEST(PacketFactoryTest, makeHelloPacket) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    unsigned int seqenceNumber = 123;

    Packet* helloPacket = factory->makeHelloPacket(source, destination, seqenceNumber);

    CHECK(helloPacket->getSource()->equals(source));
    CHECK(helloPacket->getDestination()->equals(destination));
    CHECK(helloPacket->getSender()->equals(source));
    CHECK(helloPacket->getPreviousHop()->equals(source));
    CHECK(helloPacket->getType() == PacketType::HELLO);
    LONGS_EQUAL(seqenceNumber, helloPacket->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, helloPacket->getTTL());
    LONGS_EQUAL(0, helloPacket->getPayloadLength());

    delete helloPacket;
}

TEST(PacketFactoryTest, makeDataPacket) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination(new AddressMock("destination"));
    unsigned int seqNr = 123;
    const char* payload = "I am the payload";
    unsigned int payloadSize = strlen(payload) + 1; // dont foprget the null byte for the string
    Packet* dataPacket = factory->makeDataPacket(originalSource, originalDestination, seqNr, payload, payloadSize);

    CHECK(dataPacket->getType() == PacketType::DATA);
    CHECK(dataPacket->getSource()->equals(originalSource));
    CHECK(dataPacket->getSender()->equals(originalSource));
    CHECK(dataPacket->getDestination()->equals(originalDestination));
    LONGS_EQUAL(seqNr, dataPacket->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, dataPacket->getTTL());
    LONGS_EQUAL(payloadSize, dataPacket->getPayloadLength());
    STRCMP_EQUAL(payload, dataPacket->getPayload());

    delete dataPacket;
}

TEST(PacketFactoryTest, makePANT) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination(new AddressMock("destination"));
    unsigned int sequenceNumber = 123;
    Packet* pant = factory->makePANT(source, destination, sequenceNumber);

    CHECK(pant->getType() == PacketType::PANT);
    CHECK(pant->getSource()->equals(source));
    CHECK(pant->getSender()->equals(source));
    CHECK(pant->getDestination()->equals(destination));
    LONGS_EQUAL(sequenceNumber, pant->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, pant->getTTL());
    LONGS_EQUAL(0, pant->getPayloadLength());

    delete pant;
}
