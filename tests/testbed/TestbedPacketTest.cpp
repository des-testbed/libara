/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "Testbed.h"
#include "TestbedPacket.h"
#include "TestbedPacketFactory.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedPacketTest){
    std::shared_ptr<TestbedAddress> source;
    std::shared_ptr<TestbedAddress> destination;
    TestbedPacketFactory* factory;

    void setup(){
        factory = new TestbedPacketFactory(42);

        u_char sourceMac[ETHER_ADDR_LEN] = {0x00,0x1F,0x1F,0x09,0x09,0xE2};
        source = std::make_shared<TestbedAddress>(sourceMac);

        u_char destinationMac[ETHER_ADDR_LEN] = {0x00,0x1F,0x1F,0x09,0x09,0xD2};
        destination = std::make_shared<TestbedAddress>(destinationMac);
    }

    void teardown(){
        delete factory;
    }
};

TEST(TestbedPacketTest, getSource) {
    TestbedPacket* packet = factory->makeFANT(source, destination, 42);
    CHECK(packet->getSource() == source);
    delete packet;
}

TEST(TestbedPacketTest, getDestination) {
    TestbedPacket* packet = factory->makeFANT(source, destination, 42);
    CHECK(packet->getDestination() == destination);
    delete packet;
}

TEST(TestbedPacketTest, getSender) {
    TestbedPacket* packet = factory->makeFANT(source, destination, 42);
    CHECK(packet->getSender() == source);
    delete packet;
}

TEST(TestbedPacketTest, getPreviousHop) {
    TestbedPacket* packet = factory->makeFANT(source, destination, 42);
    CHECK(packet->getPreviousHop() == source);
    delete packet;
}

TESTBED_NAMESPACE_END
