/*
 * $FU-Copyright$
 */

#include "PacketDispatcher.h"
#include "TestbedAddress.h"

#include <netinet/in.h>
#include <iostream>

TESTBED_NAMESPACE_BEGIN

_dessert_cb_results messageFromNetworkDispatcher(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_meshif_t* interface, dessert_frameid_t id) {
    //Convert dessert_msg_t to Packet
    //Get interface
    //interface -> receive (replaces syssend)
    return DESSERT_MSG_DROP; //removes packet from processing pipeline
}

void packetToNetworkDispatcher(Packet* packet, NetworkInterface* testbedInterface, std::shared_ptr<Address> recipient) {
    dessert_msg_t* message = extractDessertMessage(packet);
    addEthernetHeader(message, recipient);
    dessert_meshif_t* interface = extractDessertMeshInterface(testbedInterface);
    dessert_meshsend(message, interface);
}

_dessert_cb_results messageToNetworkDispatcher(dessert_msg_t* messageToSend, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_sysif_t *interface, dessert_frameid_t id) {
    dessert_meshsend(messageToSend, NULL);
    //TODO: log packets received via user space
    return DESSERT_MSG_DROP;
}

Packet* extractPacket(dessert_msg_t* dessertMessage) {
    ether_header* ethernetFrame = extractEthernetHeader(dessertMessage);
    routingExtension* araHeader = extractRoutingExtension(dessertMessage);

    AddressPtr source (new TestbedAddress(araHeader->ara_shost));
    AddressPtr destination (new TestbedAddress(araHeader->ara_dhost));
    AddressPtr sender (new TestbedAddress(ethernetFrame->ether_shost));

    char packetType = dessertMessage->u8;
    unsigned int sequenceNumber = dessertMessage->u16;
    int ttl = dessertMessage->ttl;

    void* payload;
    unsigned int payloadSize = ntohs(dessert_msg_getpayload(dessertMessage, &payload));

    return new Packet(source, destination, sender, packetType, sequenceNumber, ttl, (const char*)payload, payloadSize);
}

ether_header* extractEthernetHeader(dessert_msg_t* dessertMessage) {
    dessert_ext_t* extension;
    dessert_msg_getext(dessertMessage, &extension, DESSERT_EXT_ETH, 0);
    return (ether_header*) extension->data;
}

routingExtension* extractRoutingExtension(dessert_msg_t* dessertMessage) {
    dessert_ext_t* extension;
    dessert_msg_getext(dessertMessage, &extension, DESSERT_EXT_USER, 0);
    return (routingExtension*) extension->data;
}

dessert_msg_t* extractDessertMessage(Packet* packet) {
    dessert_msg_t* dessertMessage;
    dessert_msg_new(&dessertMessage);

    dessertMessage->u16 = packet->getSequenceNumber();
    dessertMessage->ttl = packet->getTTL();
    dessertMessage->u8  = packet->getType();

    dessert_ext_t* extension;

    dessert_msg_addext(dessertMessage, &extension, DESSERT_EXT_USER, DESSERT_MAXEXTDATALEN);
    struct routingExtension* araRoutingExtension = (struct routingExtension*) extension->data;

    TestbedAddressPtr sourceTestbedAddress = std::dynamic_pointer_cast<TestbedAddress>(packet->getSource());
    u_int8_t* source = sourceTestbedAddress->getDessertValue();
    TestbedAddressPtr destinationTestbedAddress = std::dynamic_pointer_cast<TestbedAddress>(packet->getDestination());
    u_int8_t* destination = destinationTestbedAddress->getDessertValue();

    memcpy(araRoutingExtension->ara_shost, source, ETHER_ADDR_LEN);
    memcpy(araRoutingExtension->ara_dhost, destination, ETHER_ADDR_LEN);

    void* payload;
    int payloadSize = packet->getPayloadLength();
    dessert_msg_addpayload(dessertMessage, &payload, payloadSize);
    memcpy(payload, packet->getPayload(), payloadSize);

    return dessertMessage;
}

void addEthernetHeader(dessert_msg_t* message, TestbedAddressPtr nextHop) {
    dessert_ext_t* extension;
    dessert_msg_addext(dessertMessage, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);

    struct ether_header* ethernetFrame = (struct ether_header*) extension->data;
    u_int8_t* source = DESSERT_LOCAL_ADDRESS;
    u_int8_t* destination = nextHop->getDessertValue();

    memcpy(ethernetFrame->ether_shost, source, ETHER_ADDR_LEN);
    memcpy(ethernetFrame->ether_dhost, destination, ETHER_ADDR_LEN);
}

dessert_meshif_t* extractDessertMeshInterface(NetworkInterface* testbedInterface) {
    //TODO: Currently is set up to send over all Mesh interfaces. Implement logic to select an interface.
    return NULL;
}

TESTBED_NAMESPACE_END
