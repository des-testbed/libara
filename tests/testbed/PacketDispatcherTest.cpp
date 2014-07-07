/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "TestbedPacketFactory.h"
#include "TestbedPacketDispatcher.h"
#include "PacketType.h"
#include "TestbedNetworkInterface.h"
#include "testAPI/mocks/ARAClientMock.h"
#include <cstring>
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(PacketDispatcherTest) {
    ARAClientMock* client;
    dessert_meshif_t* interface;
    TestbedPacketFactory *factory;

    void setup() {
        client = new ARAClientMock();
        interface = new dessert_meshif_t();
        factory = new TestbedPacketFactory(255);
    }

    void teardown() {
        delete client;
        delete interface;
        delete factory;
    }

    /**
     * Create a dessert message to test the packet marshaling process with it.
     */
    dessert_msg_t* createDessertMessage(int sequenceNumber=123, int ttl=10, char type=PacketType::FANT, u_int8_t* source=DESSERT_LOCAL_ADDRESS, u_int8_t* destination = DESSERT_BROADCAST_ADDRESS, const char** payload=nullptr) {
        dessert_msg_t* packet;
        dessert_msg_new(&packet);

        packet->u16 = htons(sequenceNumber);
        packet->ttl = ttl;
        packet->u8 = type;

        dessert_ext_t* extension;
        dessert_msg_addext(packet, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);

        struct ether_header* ethernetFrame = (struct ether_header*) extension->data;
        memcpy(ethernetFrame->ether_shost, DESSERT_LOCAL_ADDRESS, ETHER_ADDR_LEN);
        memcpy(ethernetFrame->ether_dhost, destination, ETHER_ADDR_LEN);

        dessert_msg_addext(packet, &extension, DESSERT_EXT_USER, sizeof(RoutingExtension));

        struct RoutingExtension* araHeader = (struct RoutingExtension*) extension->data;
        memcpy(araHeader->ara_shost, source, ETHER_ADDR_LEN);
        memcpy(araHeader->ara_dhost, destination, ETHER_ADDR_LEN);

        if (payload != nullptr) {
            int payloadSize = strlen(*payload)+1;
            checkDessertResult(dessert_msg_addpayload (packet, ((void**)payload), payloadSize));
        }

        return packet;
    }

    void checkDessertResult(dessert_result returnValueToCheck) {
        if (returnValueToCheck != DESSERT_OK) {
            FAIL("DESSERT return value was no DESSERT_OK");
        }
    }

    /**
     * Create a network interface for testing if some address is broadcast or the local address
     */
    TestbedNetworkInterface* createNetworkInterface() {
        //memcpy(interface->hwaddr, DESSERT_LOCAL_ADDRESS, 6);
        std::copy(DESSERT_LOCAL_ADDRESS, DESSERT_LOCAL_ADDRESS+6, interface->hwaddr);
        return new TestbedNetworkInterface(interface, client, client->getPacketFactory(), 400);
    }

    bool isSameAddress(u_char* address1, u_char* address2) {
        for (int i = 0; i < 6; ++i) {
            if(address1[i] != address2[i]){
                return false;
            }
        }
        return true;
    }
};

TEST(PacketDispatcherTest, createDessertMessage) {
    int ttl = 45;
    unsigned int sequenceNumber = 123;
    char type = PacketType::DATA;
    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl, type);
    CHECK(dessertMessage != nullptr);
    LONGS_EQUAL(sequenceNumber, ntohs(dessertMessage->u16));
    LONGS_EQUAL(ttl, dessertMessage->ttl);
    BYTES_EQUAL(type, dessertMessage->u8);
}

TEST(PacketDispatcherTest, extractSimpleFields) {
    int ttl = 45;
    unsigned int sequenceNumber = 123;
    char type = PacketType::ROUTE_FAILURE;
    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl, type);
    TestbedPacket* packet = factory->makeNewPacket(dessertMessage);

    BYTES_EQUAL(ttl, packet->getTTL());
    BYTES_EQUAL(sequenceNumber, packet->getSequenceNumber());
    BYTES_EQUAL(type, packet->getType());

    delete packet;
}

TEST(PacketDispatcherTest, extractSourceAndDestination) {
    TestbedNetworkInterface* interface = createNetworkInterface();

    int ttl = 45;
    unsigned int sequenceNumber = 123;
    char type = PacketType::FANT;
    u_int8_t* source = DESSERT_LOCAL_ADDRESS;
    u_int8_t* destination = DESSERT_BROADCAST_ADDRESS;

    dessert_msg_t* dessertMessage  = createDessertMessage(sequenceNumber, ttl, type, source, destination);
    TestbedPacket* packet = factory->makeNewPacket(dessertMessage);

    AddressPtr extractDestination = packet->getDestination();
    CHECK(extractDestination != nullptr);
    CHECK(interface->isBroadcastAddress(extractDestination));

    AddressPtr extractedSource = packet->getSource();
    CHECK(extractedSource != nullptr);
    CHECK(interface->isLocalAddress(extractedSource));

    delete packet;
    delete interface;
}
/*
TEST(PacketDispatcherTest, extractPayload) {
    const char* payload = "Hello World!";

    dessert_msg_t* dessertMessage = createDessertMessage(123, 10, PacketType::FANT, DESSERT_LOCAL_ADDRESS, DESSERT_BROADCAST_ADDRESS, &payload);
    TestbedPacket* packet = factory->makeNewPacket(dessertMessage);

    STRCMP_EQUAL(payload, packet->getPayload());
    BYTES_EQUAL(13, packet->getPayloadLength());

    delete packet;
}
*/

TEST(PacketDispatcherTest, extractPacketWithoutPayload) {
    dessert_msg_t* dessertMessage = createDessertMessage();
    TestbedPacket* packet = factory->makeNewPacket(dessertMessage);

    BYTES_EQUAL(0, packet->getPayloadLength());

    delete packet;
}

TEST(PacketDispatcherTest, packetToDessertMessage) {
    u_char sourceMAC[] = {5,186,24,3,82,1};
    AddressPtr source = std::make_shared<TestbedAddress>(sourceMAC);
    u_char destinationMAC[] = {192,168,1,1,69,18};
    AddressPtr destination = std::make_shared<TestbedAddress>(destinationMAC);

    char type = PacketType::BANT;
    unsigned int sequenceNumber = 37;
    int ttl = 42;
    const char* payload = "abcd";
    long payloadLength = 5;
    const TestbedPacket packet(source, destination, source, type, sequenceNumber, ttl, payload, payloadLength);

    dessert_msg_t* dessertMessage  = packet.toDessertMessage();

    CHECK(dessertMessage != nullptr);
    LONGS_EQUAL(sequenceNumber, ntohs(dessertMessage->u16));
    LONGS_EQUAL(ttl, dessertMessage->ttl);
    BYTES_EQUAL(type, dessertMessage->u8);

/*
    dessert_ext_t* araExtension;
    dessert_msg_getext(dessertMessage, &araExtension, DESSERT_EXT_USER, 0);
    RoutingExtension* araHeader = (RoutingExtension*) araExtension->data;
    CHECK(isSameAddress(araHeader->ara_dhost, destinationMAC));
    CHECK(isSameAddress(araHeader->ara_shost, sourceMAC));

    dessert_ext_t* extension;
    dessert_msg_getext(dessertMessage, &extension, DESSERT_EXT_ETH, 0);
    ether_header* ethernetHeader = ((ether_header*) extension->data);

    CHECK(isSameAddress(ethernetHeader->ether_dhost, destinationMAC));
    CHECK(isSameAddress(ethernetHeader->ether_shost, DESSERT_LOCAL_ADDRESS));

    const char* extractedPayload;

    //ntohs converts payload length from network byte order to host byte order
    LONGS_EQUAL(payloadLength, ntohs(dessertMessage->plen));
    dessert_msg_getpayload(dessertMessage, (void**)&extractedPayload);
    STRCMP_EQUAL(payload, extractedPayload);
    */
}

TESTBED_NAMESPACE_END
