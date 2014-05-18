/*
 * $FU-Copyright$
 */

#include "TestbedPacketFactory.h"

extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;

TESTBED_NAMESPACE_BEGIN

TestbedPacketFactory::TestbedPacketFactory(int maxHopCount) : PacketFactory(maxHopCount) { }

TestbedPacket* TestbedPacketFactory::makePacket(dessert_msg_t* message) {
    ether_header* ethernetFrame = this->getEthernetHeader(message);
    RoutingExtension* araHeader = this->getRoutingExtension(message);

    AddressPtr source = std::make_shared<TestbedAddress>(araHeader->ara_shost);
    AddressPtr destination = std::make_shared<TestbedAddress>(araHeader->ara_dhost);
    AddressPtr sender = std::make_shared<TestbedAddress>(ethernetFrame->ether_shost);

    char packetType = message->u8;
    unsigned int sequenceNumber = message->u16;
    int ttl = message->ttl;

    void* payload;
    unsigned int payloadSize = ntohs(dessert_msg_getpayload(message, &payload));

    if (payloadSize == 0){
        payload = nullptr;
    }

    return new TestbedPacket(source, destination, sender, packetType, sequenceNumber, ttl, (const char*)payload, payloadSize);
}


TestbedPacket* TestbedPacketFactory::makeNewPacket(dessert_msg_t* message) {
    return this->makePacket(message);
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(dessert_msg_t* message) {
    ether_header* ethernetFrame = this->getEthernetHeader(message);

    AddressPtr source = std::make_shared<TestbedAddress>(ethernetFrame->ether_shost);
    AddressPtr destination = std::make_shared<TestbedAddress>(ethernetFrame->ether_dhost);

    // DEBUG: std::cout << "[tapMessageToPacket] create packet from dessert message received over tap interface" << std::endl;
    // DEBUG: std::cout << "[tapMessageToPacket] source: " << source->toString() << " destination: " << destination->toString() << std::endl;

    void* payload;
    unsigned int payloadSize = ntohs(dessert_msg_getpayload(message, &payload));

    // DEBUG: std::cout << "[tapMessageToPacket] payload length is " <<  payloadSize << std::endl;

    if (payloadSize == 0){
        payload = nullptr;
    }

    return makeDataPacket(source, destination, client->getNextSequenceNumber(), (const char*)payload, payloadSize);
}


TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, this->maxHopCount, client->getNextSequenceNumber(), (const char*)payload, payloadSize);
}

TestbedPacket* TestbedPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    return new TestbedPacket(source, destination, sender, type, ttl, seqNr, payload, payloadSize);
}



dessert_msg_t* TestbedPacketFactory::makeDessertMessage(const Packet* packet, dessert_meshif_t* interface, AddressPtr nextHop) {
    dessert_msg_t* dessertMessage;
    dessert_msg_new(&dessertMessage);

    dessertMessage->u16 = packet->getSequenceNumber();
    dessertMessage->ttl = packet->getTTL();
    dessertMessage->u8  = packet->getType();

    TestbedAddressPtr source = std::dynamic_pointer_cast<TestbedAddress>(packet->getSource());
    /// DEBUG: std::cout << "[extractDessertMessage] use count of source shared_ptr after cast: " << source.use_count() << std::endl; 
    TestbedAddressPtr destination = std::dynamic_pointer_cast<TestbedAddress>(packet->getDestination());
    /// DEBUG: std::cout << "[extractDessertMessage] use count of destination shared_ptr after cast: " << destination.use_count() << std::endl; 

    /// TODO: that might be a problem 
    this->setRoutingExtension(dessertMessage, source->getDessertValue(), destination->getDessertValue());

    void* payload;
    int payloadSize = packet->getPayloadLength();
    /// DEBUG: std::cout << "[extractDessertMessage] payload length is " <<  packet->getPayloadLength() << std::endl;

    if (dessert_msg_addpayload(dessertMessage, &payload, payloadSize) == DESSERT_OK){
        memcpy(payload, packet->getPayload(), payloadSize);
    } else {
	    std::cerr << "[extractDessertMessage] error on adding payload to dessert message " << std::endl;
    }

    this->setEthernetHeader(dessertMessage, interface, nextHop);

    return dessertMessage;
}


ether_header* TestbedPacketFactory::getEthernetHeader(dessert_msg_t* message){
    dessert_ext_t* extension;
    dessert_msg_getext(message, &extension, DESSERT_EXT_ETH, 0);
    return ((ether_header*) extension->data);
}

void TestbedPacketFactory::setEthernetHeader(dessert_msg_t* message, dessert_meshif_t* interface, AddressPtr nextHop) {
    dessert_ext_t* extension;
    dessert_msg_addext(message, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);

    struct ether_header* ethernetFrame = (struct ether_header*) extension->data;
    u_int8_t* senderMac = interface->hwaddr;
    TestbedAddressPtr recipient = std::dynamic_pointer_cast<TestbedAddress>(nextHop);
    u_int8_t* nextHopMac = recipient->getDessertValue();

    std::copy(senderMac, senderMac + ETHER_ADDR_LEN, ethernetFrame->ether_shost);
    std::copy(nextHopMac, nextHopMac + ETHER_ADDR_LEN, ethernetFrame->ether_dhost);
}

RoutingExtension* TestbedPacketFactory::getRoutingExtension(dessert_msg_t* message){
    dessert_ext_t* extension;
    RoutingExtension* araHeader;

    if(dessert_msg_getext(message, &extension, DESSERT_EXT_USER, 0) == 0){
       araHeader = nullptr;
    } else {
       araHeader = (RoutingExtension*) extension->data;

    }

    return araHeader;
}

void TestbedPacketFactory::setRoutingExtension(dessert_msg_t* message, u_int8_t* source, u_int8_t* destination) {
    dessert_ext_t* extension;
    dessert_msg_addext(message, &extension, DESSERT_EXT_USER, ETHER_HDR_LEN);
    RoutingExtension* araRoutingExtension = (RoutingExtension*) extension->data;

    std::copy(source, source + ETHER_ADDR_LEN, araRoutingExtension->ara_shost);
    std::copy(destination, destination + ETHER_ADDR_LEN, araRoutingExtension->ara_dhost);
}

TESTBED_NAMESPACE_END
