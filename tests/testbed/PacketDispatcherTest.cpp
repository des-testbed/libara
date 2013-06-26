/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "PacketDispatcher.h"
#include "testbed/NetworkInterface.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(PacketDispatcherTest) {

    dessert_msg_t* createDessertMessage(int sequenceNumber, int ttl, u_int8_t* source = DESSERT_LOCAL_ADDRESS, u_int8_t* destination = DESSERT_BROADCAST_ADDRESS) {
        dessert_msg_t* packet;
        dessert_msg_new(&packet);

        packet->u16 = sequenceNumber;
        packet->ttl = ttl;
        //TODO: packet->u8 |= 0x01;

        dessert_ext_t* extension;
        dessert_msg_addext(packet, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);

        struct ether_header* ethernetFrame = (struct ether_header*) extension->data;
        memcpy(ethernetFrame->ether_shost, source, ETHER_ADDR_LEN);
        memcpy(ethernetFrame->ether_dhost, destination, ETHER_ADDR_LEN);

        dessert_msg_dummy_payload(packet, 128);
        return(packet);
    }
};

TEST(PacketDispatcherTest, createDessertMessage) {
    int ttl = 45;
    unsigned int sequenceNumber = 123;
    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl);
    CHECK(dessertMessage != NULL);
    CHECK(dessertMessage != nullptr);
    LONGS_EQUAL(sequenceNumber, dessertMessage->u16);
    LONGS_EQUAL(ttl, dessertMessage->ttl);
}

TEST(PacketDispatcherTest, extractTTLAndSequenceNumber) {
    int ttl = 45;
    unsigned int sequenceNumber = 123;
    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl);
    Packet* packet = extractPacket(dessertMessage);

    BYTES_EQUAL(ttl, packet->getTTL());
    BYTES_EQUAL(sequenceNumber, packet->getSequenceNumber());

    delete packet;
}

IGNORE_TEST(PacketDispatcherTest, extractSourceAndDestination) {
    //FIXME Continue writing test when NetworkInterface works
    int ttl = 45;
    unsigned int sequenceNumber = 123;
    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl,  DESSERT_LOCAL_ADDRESS, DESSERT_BROADCAST_ADDRESS);
    Packet* packet = extractPacket(dessertMessage);

    CHECK(packet)

    delete packet;
}

TESTBED_NAMESPACE_END
