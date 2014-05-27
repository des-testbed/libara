/*
 * $FU-Copyright$
 */

#include "TestbedPacketFactory.h"

extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;

TESTBED_NAMESPACE_BEGIN

TestbedPacketFactory::TestbedPacketFactory(int maxHopCount) : PacketFactory(maxHopCount) {}

TestbedPacket* TestbedPacketFactory::makePacket(dessert_msg_t* message) {
    ether_header* ethernetFrame = this->getEthernetHeader(message);
    RoutingExtension* araHeader = this->getRoutingExtension(message);

    AddressPtr source = std::make_shared<TestbedAddress>(araHeader->ara_shost);
    AddressPtr destination = std::make_shared<TestbedAddress>(araHeader->ara_dhost);
    AddressPtr sender = std::make_shared<TestbedAddress>(ethernetFrame->ether_shost);

    char packetType = message->u8;
    unsigned int sequenceNumber = message->u16;
    int ttl = message->ttl;
 
    struct ether_header *payload = nullptr;
    int payloadSize = 0;

    /**
     * TODO: We should check if we should add payload by libdessert  even if there is no
     * paylaod in the first place (well, sounds absurd)
     */
    if (message->plen > 0) {
        struct ether_header *payload = nullptr;
        payloadSize = getPayload(message, &payload);
    }

    return new TestbedPacket(source, destination, sender, packetType, sequenceNumber, ttl, payload, payloadSize);
}



TestbedPacket* TestbedPacketFactory::makeNewPacket(dessert_msg_t* message) {
    return this->makePacket(message);
}


TestbedPacket* TestbedPacketFactory::makeDataPacket(dessert_msg_t* message) {
    ether_header* ethernetFrame = this->getEthernetHeader(message);

    AddressPtr source = std::make_shared<TestbedAddress>(ethernetFrame->ether_shost);
    AddressPtr destination = std::make_shared<TestbedAddress>(ethernetFrame->ether_dhost);

    struct ether_header *payload;
    int payloadSize = getPayload(message, &payload);

    return makeDataPacket(source, destination, client->getNextSequenceNumber(), payload, payloadSize);
}


Packet* TestbedPacketFactory::makeClone(const Packet* packet) {
    const TestbedPacket *originalPacket = dynamic_cast<const TestbedPacket*>(packet);
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getTTL(), originalPacket->getDessertPayload(), originalPacket->getPayloadLength(), originalPacket->getPreviousHop());
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, struct ether_header* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, sequenceNumber, this->maxHopCount, payload, payloadSize);
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, sequenceNumber, this->maxHopCount, (const char*)payload, payloadSize);
}

TestbedPacket* TestbedPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, struct ether_header* payload, unsigned int payloadSize, AddressPtr previousHop) {
    return new TestbedPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
}

TestbedPacket* TestbedPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    return new TestbedPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
}



dessert_msg_t* TestbedPacketFactory::makeDessertMessage(const Packet* packet, dessert_meshif_t* interface, AddressPtr nextHop) {
    assert(packet != nullptr);

    /// check the type of the packet
    if (packet->getType() == PacketType::FANT) {
        return this->makeFANT(packet);
    } else if (packet->getType() == PacketType::BANT) {
        return this->makeBANT(packet);
    } else if (packet->getType() == PacketType::DATA){
        return this->makeDataPacket(packet);
    } else {
        throw Exception("unsupported packet type");
    }
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

/**
 * This method is a copy of the function dessert_msg_ethdecap() from libdessert.
 * However, we changed the calls to malloc and free to new and delete. We also
 * replaced memcpy with std::copy.
 */
int TestbedPacketFactory::getPayload(dessert_msg_t* message, struct ether_header **payload) {
    dessert_ext_t* extension;

    /// create message 
    uint32_t length = ntohs(message->plen) + ETHER_HDR_LEN;
    *payload = new struct ether_header[length];

    if (*payload == nullptr) {
        return -1;
    }

    /// copy the header 
    if (dessert_msg_getext(message, &extension, DESSERT_EXT_ETH, 0) == -1) {
        delete[] payload;
        return -1;
    }

    /// TODO: 
    std::copy(extension->data, extension->data + ETHER_HDR_LEN, reinterpret_cast<uint8_t*>(*payload));

    std::copy((((uint8_t*) message) + ntohs(message->hlen)), 
        (((uint8_t*) message) + ntohs(message->hlen)) + ntohs(message->plen), 
         (uint8_t*)(*payload) + ETHER_HDR_LEN);

    return length;
}


dessert_msg_t* TestbedPacketFactory::makeFANT(const Packet *packet){
    return makeAntAgent(packet);
}

dessert_msg_t* TestbedPacketFactory::makeAntAgent(const Packet *packet){
    ara_address_t address;
    dessert_msg_t* ant = nullptr;
    dessert_ext_t* extension = nullptr;
    struct ether_header* ethernetHeader = nullptr;

    dessert_msg_new(&ant);

    /// set sequence number, ttl and type
    ant->u16 = htons(packet->getSequenceNumber());
    ant->ttl = (uint8_t)packet->getTTL();
    ant->u8 |= packet->getType();

    /// add user extension
    dessert_msg_addext(ant, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);
    ethernetHeader = (struct ether_header*) extension->data;
    memcpy(ethernetHeader->ether_shost, DESSERT_LOCAL_ADDRESS, ETHER_ADDR_LEN);
    memcpy(ethernetHeader->ether_dhost, DESSERT_BROADCAST_ADDRESS, ETHER_ADDR_LEN);

    if (packet->getType() == PacketType::FANT) {
        dessert_msg_addext(ant, &extension, ARA_EXT_FANT, ETHER_ADDR_LEN + 4);
    } else if(packet->getType() == PacketType::BANT) {
        dessert_msg_addext(ant, &extension, ARA_EXT_BANT, ETHER_ADDR_LEN + 4);
    } else {
        throw Exception("Unsupported packet type in makeAntAgent() method");
    } 

    /// get the address
    std::shared_ptr<TestbedAddress> testbedAddress = std::dynamic_pointer_cast<TestbedAddress>(packet->getSource());
    std::copy(testbedAddress->getDessertValue(), testbedAddress->getDessertValue() + ETHER_ADDR_LEN, address);
    memcpy(extension->data, address, sizeof(ara_address_t));

    if (packet->getType() == PacketType::FANT) {
        memcpy(extension->data + ETHER_ADDR_LEN, "FANT", 4);
    } else if(packet->getType() == PacketType::BANT) {
        memcpy(extension->data + ETHER_ADDR_LEN, "BANT", 4);
//      ara_maintainroute_stamp(addr);
    } else {
        throw Exception("Unsupported packet type in makeAntAgent() method");
    }
    /// TODO: Check
    /* for loop vs duplicate detection */
//  dessert_msg_trace_initiate(bant, DESSERT_EXT_TRACE_REQ, DESSERT_MSG_TRACE_HOST);

    /// TODO: DES-ARA sets ara_ant_size to 128, should better check this
    dessert_msg_dummy_payload(ant, 128);

    return ant;
}

dessert_msg_t* TestbedPacketFactory::makeBANT(const Packet *packet){
    return makeAntAgent(packet);
}

TESTBED_NAMESPACE_END
