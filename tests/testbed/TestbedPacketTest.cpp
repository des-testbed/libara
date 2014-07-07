/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "PacketType.h"
#include "TestbedPacket.h"
#include "TestbedAddress.h"
#include "CppUTest/TestHarness.h"

TESTBED_NAMESPACE_BEGIN


TEST_GROUP(TestbedPacketTest) {
    std::shared_ptr<TestbedAddress> source;
    std::shared_ptr<TestbedAddress> destination;

    void setup(){
        u_char sourceMac[ETHER_ADDR_LEN] = {0x00,0x1F,0x1F,0x09,0x09,0xE2};
        source = std::make_shared<TestbedAddress>(sourceMac);
    
        u_char destinationMac[ETHER_ADDR_LEN] = {0x00,0x1F,0x1F,0x09,0x09,0xD2};
        destination = std::make_shared<TestbedAddress>(destinationMac);
    }

    TestbedPacket* createPacket(int ttl = 15, unsigned int seqNr = 42, char type = PacketType::DATA){
        return new TestbedPacket(source, destination, source, type, seqNr, ttl);
    }
};

TEST(TestbedPacketTest, getSource) {
    TestbedPacket* packet = createPacket(); 
    CHECK(packet->getSource() == source);
    delete packet;
}

TEST(TestbedPacketTest, getDestination) {
    TestbedPacket* packet = createPacket(); 
    CHECK(packet->getDestination() == destination);
    delete packet;
}

TEST(TestbedPacketTest, getSender) {
    TestbedPacket* packet = createPacket(); 
    CHECK(packet->getSender() == source);
    delete packet;
}

TEST(TestbedPacketTest, getPreviousHop) {
    TestbedPacket* packet = createPacket(); 
    CHECK(packet->getPreviousHop() == source);
    delete packet;
}

TEST(TestbedPacketTest, getMessage) {
    unsigned int seqNr = 42;
    int ttl = 15;
    TestbedPacket* packet = createPacket(ttl, seqNr); 

    dessert_msg_t* message = packet->getMessage();
    dessert_msg_t* messageToCompare;
    dessert_ext_t* extension = nullptr;
    struct ether_header* ethernetHeader = nullptr;

    if(dessert_msg_new(&messageToCompare)){
        messageToCompare->u16 = htons(seqNr);
        messageToCompare->ttl = (uint8_t)ttl;

        CHECK(message->u16 = messageToCompare->u16);
        CHECK(message->ttl = messageToCompare->ttl);

        // TODO: Continue!!!

        dessert_msg_destroy(messageToCompare);
    }

    delete packet;
}

TEST(TestbedPacketTest, makeDataPacket) {
    unsigned int seqNr = 42;
    int ttl = 15;

    TestbedPacket* packet = createPacket(ttl, seqNr, PacketType::FANT); 
    dessert_msg_t* ant = packet->toDessertMessage();

    CHECK(ntohs(ant->u16) == seqNr);
    CHECK(ant->ttl == ttl);
    CHECK(ant->u8 & PacketType::FANT);

    dessert_ext_t* extension = nullptr;

//    CHECK(dessert_msg_getext(ant, &extension, ARA_EXT_FANT, 0) > 0);
//    STRCMP_EQUAL("FANT", extension->data);

    CHECK(dessert_msg_getext(ant, &extension, DESSERT_EXT_ETH, 0) > 0);
    struct ether_header* ethernetHeader = (struct ether_header*) extension->data;
    std::cerr << ethernetHeader->ether_shost << std::endl;
//    std::cerr << ethernetHeader->ether_dhost << std::endl;
//    CHECK(ethernetHeader->ether_dhost)
//
    delete packet;
}

TESTBED_NAMESPACE_END
