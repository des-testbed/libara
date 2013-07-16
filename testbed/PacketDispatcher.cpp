/*
 * $FU-Copyright$
 */

#include "PacketDispatcher.h"
#include "TestbedAddress.h"

#include <netinet/in.h>
#include <iostream>


TESTBED_NAMESPACE_BEGIN

/**
 * Stores a map of network interface pointers by dessert interface pointers.
 */
NetworkInterfaceMap networkInterfaces;

_dessert_cb_results messageFromMeshInterfaceDispatcher(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_meshif_t* interface, dessert_frameid_t id) {

    if(!isARAMessage(messageReceived)) {
        dessert_debug("dessertmessage %u has null routingExtension, sending to sys", ntohs(messageReceived->u16));
        void* payload;
        dessert_msg_getpayload(messageReceived, &payload);
        dessert_syssend(payload, messageReceived->plen);
        return DESSERT_MSG_DROP;
    }
    else {
        return DESSERT_MSG_KEEP;
    }

}

bool isARAMessage(dessert_msg_t* message) {
    dessert_ext_t* extension;
    return dessert_msg_getext(message, &extension, DESSERT_EXT_USER, 0) != 0;
}

_dessert_cb_results araMessageDispatcher(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_meshif_t* interface, dessert_frameid_t id) {
    Packet* packet = extractPacket(messageReceived);
    std::cout << "Recieved Packet: " << packet->getPayload() << std::endl;
    extractNetworkInterface(interface)->receive(packet);
    return DESSERT_MSG_DROP;
}

void packetToMeshInterfaceDispatcher(const Packet* packet, NetworkInterface* testbedInterface, std::shared_ptr<Address> recipient) {
    dessert_msg_t* message = extractDessertMessage(packet);
    addEthernetHeader(message, recipient);
    dessert_meshif_t* interface = testbedInterface->getDessertPointer();
    dessert_meshsend(message, interface);
}

_dessert_cb_results messageFromTapInterfaceDispatcher(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_sysif_t *interface, dessert_frameid_t id) {
    std::cout << "Received packet " << messageReceived->u16 << " of size " << ntohs(messageReceived->plen) << " bits via tap, sending to client" << std::endl;
    Packet* packet = extractPacket(messageReceived);

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

dessert_msg_t* extractDessertMessage(const Packet* packet) {
    dessert_msg_t* dessertMessage;
    dessert_msg_new(&dessertMessage);

    dessertMessage->u16 = packet->getSequenceNumber();
    dessertMessage->ttl = packet->getTTL();
    dessertMessage->u8  = packet->getType();

    dessert_ext_t* extension;

    addEthernetHeader(dessertMessage, packet->getDestination());

    dessert_msg_addext(dessertMessage, &extension, DESSERT_EXT_USER, ETHER_HDR_LEN);
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

void addEthernetHeader(dessert_msg_t* message, AddressPtr nextHop) {
    dessert_ext_t* extension;
    dessert_msg_addext(message, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);

    struct ether_header* ethernetFrame = (struct ether_header*) extension->data;
    u_int8_t* source = DESSERT_LOCAL_ADDRESS;
    TestbedAddressPtr recipient = std::dynamic_pointer_cast<TestbedAddress>(nextHop);
    u_int8_t* destination = recipient->getDessertValue();

    memcpy(ethernetFrame->ether_shost, source, ETHER_ADDR_LEN);
    memcpy(ethernetFrame->ether_dhost, destination, ETHER_ADDR_LEN);
}

NetworkInterface* extractNetworkInterface(dessert_meshif_t* dessertInterface) {
    std::unordered_map<dessert_meshif_t*, NetworkInterface*>::const_iterator got = networkInterfaces.find(dessertInterface);
    if(got == networkInterfaces.end()) {
       return(NULL);
    }
    else {
        return got->second;
    }
}

TESTBED_NAMESPACE_END
