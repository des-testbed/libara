/*
 * $FU-Copyright$
 */

#include "TestbedPacketDispatcher.h"
#include "TestbedAddress.h"

#include <netinet/in.h>

TESTBED_NAMESPACE_BEGIN

/**
 * Stores a map of network interface pointers by dessert interface pointers.
 */
NetworkInterfaceMap networkInterfaces;

_dessert_cb_results messageFromMeshInterfaceDispatcher(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_meshif_t* interface, dessert_frameid_t id) {
    /// DEBUG: 
    std::cout << "[messageFromMeshInterfaceDispatcher] heyho i've got a packet" << std::endl;
    Packet* packet = extractPacket(messageReceived);
    std::cout << " Packet Dump " << std::endl;
    std::cout << packet->toString() << std::endl;
    TestbedNetworkInterface* networkInterface = extractNetworkInterface(interface);
    networkInterface->receive(packet);
    return DESSERT_MSG_DROP;
}

void packetToMeshInterfaceDispatcher(const Packet* packet, TestbedNetworkInterface* testbedInterface, std::shared_ptr<Address> recipient) {
    // DEBUG: 
    std::cout << "[packetToMeshInterfaceDispatcher] heyho i've got a packet" << std::endl;
    dessert_msg_t* message = extractDessertMessage(packet);
    dessert_meshif_t* interface = testbedInterface->getDessertPointer();
    addEthernetHeader(message, interface, recipient);
    dessert_meshsend(message, interface);
}

Packet* extractPacket(dessert_msg_t* dessertMessage) {
    ether_header* ethernetFrame = extractEthernetHeader(dessertMessage);
    routingExtension* araHeader = extractRoutingExtension(dessertMessage);

    AddressPtr source = std::make_shared<TestbedAddress>(araHeader->ara_shost);
    AddressPtr destination = std::make_shared<TestbedAddress>(araHeader->ara_dhost);
    AddressPtr sender = std::make_shared<TestbedAddress>(ethernetFrame->ether_shost);

    char packetType = dessertMessage->u8;
    unsigned int sequenceNumber = dessertMessage->u16;
    int ttl = dessertMessage->ttl;

    void* payload;
    unsigned int payloadSize = ntohs(dessert_msg_getpayload(dessertMessage, &payload));
    std::cout << "[extractPacket] payload length is " <<  payloadSize << std::endl;

    if (payloadSize == 0){
        payload = nullptr;
    }

    return new Packet(source, destination, sender, packetType, sequenceNumber, ttl, (const char*)payload, payloadSize);
}

Packet* tapMessageToPacket(dessert_msg_t* dessertMessage, std::weak_ptr<TestbedARAClient> testbedClient) {
    auto client = testbedClient.lock();
    ether_header* ethernetFrame = extractEthernetHeader(dessertMessage);

    AddressPtr source = std::dynamic_pointer_cast<Address>(std::make_shared<TestbedAddress>(ethernetFrame->ether_shost));
    AddressPtr destination = std::dynamic_pointer_cast<Address>(std::make_shared<TestbedAddress>(ethernetFrame->ether_dhost));
    std::cout << "[tapMessageToPacket] create packet from dessert message received over tap interface" << std::endl;
    std::cout << "[tapMessageToPacket] source: " << source->toString() << " destination: " << destination->toString() << std::endl;

    void* payload;
    unsigned int payloadSize = ntohs(dessert_msg_getpayload(dessertMessage, &payload));
    std::cout << "[tapMessageToPacket] payload length is " <<  payloadSize << std::endl;
    if (payloadSize == 0){
        payload = nullptr;
    }

    return client->getPacketFactory()->makeDataPacket(source, destination, client->getNextSequenceNumber(), (const char*)payload, payloadSize);
}

ether_header* extractEthernetHeader(dessert_msg_t* dessertMessage) {
    dessert_ext_t* extension;
    dessert_msg_getext(dessertMessage, &extension, DESSERT_EXT_ETH, 0);
    return (ether_header*) extension->data;
}

routingExtension* extractRoutingExtension(dessert_msg_t* dessertMessage) {
    dessert_ext_t* extension;

    if(dessert_msg_getext(dessertMessage, &extension, DESSERT_EXT_USER, 0) == 0){
        return nullptr;
    }

    return (routingExtension*) extension->data;
}

dessert_msg_t* extractDessertMessage(const Packet* packet) {
    dessert_msg_t* dessertMessage;
    dessert_msg_new(&dessertMessage);

    dessertMessage->u16 = packet->getSequenceNumber();
    dessertMessage->ttl = packet->getTTL();
    dessertMessage->u8  = packet->getType();
   
    std::cout << "[extractDessertMessage] use count of source shared_ptr: " << packet->getSource().use_count() << std::endl; 
    std::cout << "[extractDessertMessage] use count of destination shared_ptr: " << packet->getDestination().use_count() << std::endl; 

    TestbedAddressPtr source = std::dynamic_pointer_cast<TestbedAddress>(packet->getSource());
    std::cout << "[extractDessertMessage] use count of source shared_ptr after cast: " << source.use_count() << std::endl; 
    TestbedAddressPtr destination = std::dynamic_pointer_cast<TestbedAddress>(packet->getDestination());
    std::cout << "[extractDessertMessage] use count of destination shared_ptr after cast: " << destination.use_count() << std::endl; 

    addRoutingExtension(dessertMessage, source->getDessertValue(), destination->getDessertValue());

    //std::cout << "||Extract DES-SERT|| source: " << sourceTestbedAddress.get()->toString() << " destination: " << destinationTestbedAddress.get()->toString() << " sender: " << packet->getSenderString() << std::endl;

    void* payload;
    int payloadSize = packet->getPayloadLength();
    std::cout << "[extractDessertMessage] payload length is " <<  packet->getPayloadLength() << std::endl;

    if (dessert_msg_addpayload(dessertMessage, &payload, payloadSize) == DESSERT_OK){
        memcpy(payload, packet->getPayload(), payloadSize);
        //std::copy(packet->getPayload(), packet->getPayload() + payloadSize, payload);
    } else {
	std::cerr << "[extractDessertMessage] error on adding payload to dessert message " << std::endl;
    }

    return dessertMessage;
}

void addEthernetHeader(dessert_msg_t* message, dessert_meshif_t* interface, AddressPtr nextHop) {
    dessert_ext_t* extension;
    dessert_msg_addext(message, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);

    struct ether_header* ethernetFrame = (struct ether_header*) extension->data;
    u_int8_t* senderMac = interface->hwaddr;
    TestbedAddressPtr recipient = std::dynamic_pointer_cast<TestbedAddress>(nextHop);
    u_int8_t* nextHopMac = recipient->getDessertValue();

    std::copy(senderMac, senderMac + ETHER_ADDR_LEN, ethernetFrame->ether_shost);
    std::copy(nextHopMac, nextHopMac + ETHER_ADDR_LEN, ethernetFrame->ether_dhost);
}

void addRoutingExtension(dessert_msg_t* message, u_int8_t* source, u_int8_t* destination) {
    dessert_ext_t* extension;
    dessert_msg_addext(message, &extension, DESSERT_EXT_USER, ETHER_HDR_LEN);
    struct routingExtension* araRoutingExtension = (struct routingExtension*) extension->data;

    std::copy(source, source + ETHER_ADDR_LEN, araRoutingExtension->ara_shost);
    std::copy(destination, destination + ETHER_ADDR_LEN, araRoutingExtension->ara_dhost);
}

TestbedNetworkInterface* extractNetworkInterface(dessert_meshif_t* dessertInterface) {
    std::unordered_map<dessert_meshif_t*, TestbedNetworkInterface*>::const_iterator got = networkInterfaces.find(dessertInterface);

    if (got == networkInterfaces.end()) {
        return nullptr;
    } 

    return got->second;
}

TESTBED_NAMESPACE_END
