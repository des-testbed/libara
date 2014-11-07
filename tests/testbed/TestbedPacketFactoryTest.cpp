/*
 * * $FU-Copyright$
 * */
#include "Testbed.h"
#include "CppUTest/TestHarness.h"
#include "TestbedPacketFactory.h"
#include "testAPI/mocks/AddressMock.h"
#include "TestbedPacket.h"
#include "TestbedAddress.h"
#include "PacketType.h"
#include <cstring>

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedPacketFactoryTest) {
    TestbedPacketFactory* factory;
    int maximumHopCount= 255;

    void setup() {
        factory = new TestbedPacketFactory(maximumHopCount);
    }

    void teardown() {
        delete factory;
    }
};

TEST(TestbedPacketFactoryTest, makeFANT) {
    u_char sourceMac[ETHER_ADDR_LEN] = {0x00, 0x1f, 0x1f, 0x09, 0x09, 0xe2};
    TestbedAddressPtr source = std::make_shared<TestbedAddress>(sourceMac);

    u_char destinationMac[ETHER_ADDR_LEN] = {0xfe, 0x0d, 0xb9, 0x14, 0xd8, 0xc8};
    TestbedAddressPtr destination = std::make_shared<TestbedAddress>(destinationMac);

    unsigned int newSequenceNumber = 242342;
    TestbedPacket* fant = factory->makeFANT(source, destination, newSequenceNumber);

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




TESTBED_NAMESPACE_END
