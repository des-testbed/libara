/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "PacketDispatcher.h"
#include "PacketType.h"
#include "testbed/NetworkInterface.h"
#include "testAPI/mocks/ARAClientMock.h"
#include <cstring>

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(PacketDispatcherTest) {
    ARAClientMock* client;

    void setup() {
        client = new ARAClientMock();
    }

    void teardown() {
        delete client;
    }

    dessert_msg_t* createDessertMessage(int sequenceNumber, int ttl, char type = PacketType::FANT, u_int8_t* source = DESSERT_LOCAL_ADDRESS, u_int8_t* destination = DESSERT_BROADCAST_ADDRESS, const char* payload = nullptr) {
        dessert_msg_t* packet;
        dessert_msg_new(&packet);

        packet->u16 = sequenceNumber;
        packet->ttl = ttl;
        packet->u8 = type;

        dessert_ext_t* extension;
        dessert_msg_addext(packet, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);

        struct ether_header* ethernetFrame = (struct ether_header*) extension->data;
        memcpy(ethernetFrame->ether_shost, source, ETHER_ADDR_LEN);
        memcpy(ethernetFrame->ether_dhost, destination, ETHER_ADDR_LEN);

        void** voidPayload = (void**)&payload;
        dessert_msg_addpayload (packet, voidPayload, strlen(payload)+1);
        return(packet);
    }

    NetworkInterface* createNetworkInterface() {
        return new NetworkInterface(client, client->getPacketFactory(), 400);
    }
};

TEST(PacketDispatcherTest, createDessertMessage) {
    int ttl = 45;
    unsigned int sequenceNumber = 123;
    char type = PacketType::DATA;
    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl, type);
    CHECK(dessertMessage != NULL);
    CHECK(dessertMessage != nullptr);
    LONGS_EQUAL(sequenceNumber, dessertMessage->u16);
    LONGS_EQUAL(ttl, dessertMessage->ttl);
    BYTES_EQUAL(type, dessertMessage->u8);
}

TEST(PacketDispatcherTest, extractSimpleFields) {
    int ttl = 45;
    unsigned int sequenceNumber = 123;
    char type = PacketType::ROUTE_FAILURE;
    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl, type);
    Packet* packet = extractPacket(dessertMessage);

    BYTES_EQUAL(ttl, packet->getTTL());
    BYTES_EQUAL(sequenceNumber, packet->getSequenceNumber());
    BYTES_EQUAL(type, packet->getType());

    delete packet;
}

TEST(PacketDispatcherTest, extractSourceAndDestination) {
    NetworkInterface* interface = createNetworkInterface();

    int ttl = 45;
    unsigned int sequenceNumber = 123;
    char type = PacketType::FANT;

    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl, type, DESSERT_LOCAL_ADDRESS, DESSERT_BROADCAST_ADDRESS);
    Packet* packet = extractPacket(dessertMessage);

    AddressPtr destination = packet->getDestination();
    CHECK(destination != nullptr);
    CHECK(interface->isBroadcastAddress(destination));

    AddressPtr source = packet->getSource();
    CHECK(source != nullptr);
    CHECK(interface->getLocalAddress()->equals(source));

    delete packet;
    delete interface;
}

TEST(PacketDispatcherTest, extractPayload) {
    const char* payload = "Hello World!";

    dessert_msg_t* dessertMessage  = createDessertMessage(123, 10, PacketType::FANT, DESSERT_LOCAL_ADDRESS, DESSERT_BROADCAST_ADDRESS, payload);
    Packet* packet = extractPacket(dessertMessage);

    STRCMP_EQUAL(payload, packet->getPayload());
    BYTES_EQUAL(13, packet->getPayloadLength());
}

TESTBED_NAMESPACE_END
