/*
 * $FU-Copyright$
 */

#include "TestbedPacketFactory.h"

extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;

TESTBED_NAMESPACE_BEGIN

TestbedPacketFactory::TestbedPacketFactory(int maxHopCount) : PacketFactory(maxHopCount) {}

TestbedPacket* TestbedPacketFactory::makePacket(dessert_msg_t* message) {
    /// obtain the ethernet header
    dessert_ext_t* extension = nullptr;
    dessert_msg_getext(message, &extension, DESSERT_EXT_ETH, 0);
    ether_header* ethernetFrame = (ether_header*) extension->data;

    /// get the sender
    TestbedAddressPtr source = std::make_shared<TestbedAddress>(ethernetFrame->ether_shost);
    TestbedAddressPtr destination = std::make_shared<TestbedAddress>(ethernetFrame->ether_dhost);

    /// get the ARA routing extension
    /*
    dessert_ext_t* routingExtension = nullptr;
    dessert_msg_getext(message, &routingExtension, DESSERT_EXT_USER, 0);
    RoutingExtension* araHeader = (RoutingExtension*) routingExtension->data;
    TestbedAddressPtr source = std::make_shared<TestbedAddress>(araHeader->ara_shost);
    TestbedAddressPtr destination = std::make_shared<TestbedAddress>(araHeader->ara_dhost);
*/

    // TODO: check if we should do a ntohs/htons
    char packetType = message->u8;
    // 
    unsigned int sequenceNumber = ntohs(message->u16);
    // TODO: check if we should do a ntohs/htons
    int ttl = message->ttl;
    // TODO: check the payloadsize (hlen, hlen + plen, ...?)
    int payloadSize = message->hlen;

    return new TestbedPacket(source, destination, source, packetType, sequenceNumber, ttl, message, payloadSize);
}

TestbedPacket* TestbedPacketFactory::makeNewPacket(dessert_msg_t* message) {
    return this->makePacket(message);
}

Packet* TestbedPacketFactory::makeClone(const Packet* packet) {
    const TestbedPacket *originalPacket = dynamic_cast<const TestbedPacket*>(packet);
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getTTL(), originalPacket->getMessage(), originalPacket->getPayloadLength(), originalPacket->getPreviousHop());
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, dessert_msg_t* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, sequenceNumber, this->maxHopCount, payload, payloadSize);
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, sequenceNumber, this->maxHopCount, (const char*)payload, payloadSize);
}

TestbedPacket* TestbedPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, dessert_msg_t* payload, unsigned int payloadSize, AddressPtr previousHop) {
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
        /// first we have to convert the packet
        const TestbedPacket* testbedPacket = dynamic_cast<const TestbedPacket*>(packet);
        /// simply return the previously stored dessert message 
        dessert_msg_t* message = testbedPacket->getMessage();
        /// TODO: update the data, sequence number, etc.
        
        return message;
    } else {
        throw Exception("unsupported packet type");
    }
}

dessert_msg_t* TestbedPacketFactory::makeFANT(const Packet *packet){
    return makeAntAgent(packet);
}

dessert_msg_t* TestbedPacketFactory::makeBANT(const Packet *packet){
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

TESTBED_NAMESPACE_END
