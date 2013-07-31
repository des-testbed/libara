/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EARAPacketFactory.h"
#include "EARAPacket.h"

#include "testAPI/mocks/AddressMock.h"
#include "PacketType.h"

#include <cstring>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EARAPacketFactoryTest) {
    EARAPacketFactory* factory;
    int maximumHopCount= 15;

    void setup() {
        factory = new EARAPacketFactory(maximumHopCount);
    }

    void teardown() {
        delete factory;
    }

    bool isEARAPacket(Packet* packet) {
        return dynamic_cast<EARAPacket*>(packet) != NULL;
    }
};

/**
 * This test just checks that the EARAPacketFactoryTest implementation always returns
 * instances of EARAPacket. The actual content of the created packets is tested in the
 * normal PacketFactoryTest.
 */
TEST(EARAPacketFactoryTest, returnEARAPacketInstances) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    int seqNr = 3;
    int ttl = 15;
    const char* payload = "Hello World";
    size_t payloadSize = strlen(payload);

    Packet somePacket = Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    unsigned int newSequenceNumber = 242342;

    Packet* dataPacket = factory->makeDataPacket(source, destination, 123, payload, payloadSize);
    CHECK(isEARAPacket(dataPacket));
    delete dataPacket;

    Packet* fant = factory->makeFANT(source, destination, newSequenceNumber);
    CHECK(isEARAPacket(fant));
    delete fant;

    Packet* bant = factory->makeBANT(&somePacket, 123);
    CHECK(isEARAPacket(bant));
    delete bant;

    Packet* duplicateWarningPacket = factory->makeDuplicateWarningPacket(&somePacket, sender, 123);
    CHECK(isEARAPacket(duplicateWarningPacket));
    delete duplicateWarningPacket;

    Packet* acknowledgmentPacket = factory->makeAcknowledgmentPacket(&somePacket, sender);
    CHECK(isEARAPacket(acknowledgmentPacket));
    delete acknowledgmentPacket;

    Packet* routeFailurePacket = factory->makeRouteFailurePacket(source, destination, 123);
    CHECK(isEARAPacket(routeFailurePacket));
    delete routeFailurePacket;

    Packet* helloPacket = factory->makeHelloPacket(source, destination, 123);
    CHECK(isEARAPacket(helloPacket));
    delete helloPacket;

    Packet* pant = factory->makePANT(source, destination, 123);
    CHECK(isEARAPacket(pant));
    delete pant;
}

TEST(EARAPacketFactoryTest, cloneCopiesTheEnergyValue) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    unsigned int type = PacketType::DATA;
    int seqNr = 3;
    int ttl = 15;
    const char* payload = "Hello World";
    size_t payloadSize = strlen(payload);
    int minimumEnergyValue = 30;
    int maxEnergyValue = 50;

    EARAPacket originalPacket = EARAPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    originalPacket.addEnergyValue(minimumEnergyValue);
    originalPacket.addEnergyValue(maxEnergyValue);

    EARAPacket* clone = (EARAPacket*) factory->makeClone(&originalPacket);
    LONGS_EQUAL(originalPacket.getMinimumEnergyValue(), clone->getMinimumEnergyValue());
    LONGS_EQUAL(originalPacket.getTotalEnergyValue(), clone->getTotalEnergyValue());

    delete clone;
}

TEST(EARAPacketFactoryTest, makePEANT) {
    AddressPtr source (new AddressMock("source"));
    unsigned int sequenceNumber = 123;
    Packet* peant = factory->makePEANT(source, sequenceNumber);

    CHECK(peant->getType() == PacketType::PEANT);
    CHECK(peant->getSource()->equals(source));
    CHECK(peant->getSender()->equals(source));

    // there is no real destination in a PEANT so it doesn't matter what address we choose here
    // however, we need to have something in the address field so I chose the source of the PEANt
    // itself so nobody will ever be tempted to process this packet like it was meant only to him
    CHECK(peant->getDestination()->equals(source));

    LONGS_EQUAL(sequenceNumber, peant->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, peant->getTTL());
    LONGS_EQUAL(0, peant->getPayloadLength());

    delete peant;
}
