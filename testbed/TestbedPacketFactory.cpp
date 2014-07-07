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

    TestbedPacket* packet = new TestbedPacket(source, destination, source, packetType, sequenceNumber, ttl);
    packet->setMessage(message);
    packet->setPayloadLength(payloadSize);

    return packet;
}

TestbedPacket* TestbedPacketFactory::makeNewPacket(dessert_msg_t* message) {
    return this->makePacket(message);
}

TestbedPacket* TestbedPacketFactory::makeClone(const TestbedPacket* packet) {
    if (packet->getMessage() != nullptr){
        TestbedPacket* result = this->makePacket(packet->getSource(), packet->getDestination(), packet->getSender(), packet->getType(), packet->getSequenceNumber(), packet->getTTL());
        result->setPreviousHop(packet->getPreviousHop());
        result->setMessage(packet->getMessage());
        result->setPayloadLength(packet->getPayloadLength());
        return result;
    }
    return makePacket(packet->getSource(), packet->getDestination(), packet->getSender(), packet->getType(), packet->getSequenceNumber(), packet->getTTL(), packet->getPayload(), packet->getPayloadLength(), packet->getPreviousHop());
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, dessert_msg_t* payload, unsigned int payloadSize) {
    TestbedPacket* packet = this->makePacket(source, destination, source, PacketType::DATA, sequenceNumber, maxHopCount);
    packet->setMessage(payload);
    packet->setPayloadLength(payloadSize);
    return packet;
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, sequenceNumber, this->maxHopCount, (const char*)payload, payloadSize);
}
/*
TestbedPacket* TestbedPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, dessert_msg_t* payload, unsigned int payloadSize, AddressPtr previousHop) {
    TestbedPacket* packet = new TestbedPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    if(previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }
    return packet;
}
*/

TestbedPacket* TestbedPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    TestbedPacket* packet = new TestbedPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    if(previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }
    return packet;
}

TestbedPacket* TestbedPacketFactory::makeFANT(TestbedAddressPtr source, TestbedAddressPtr destination, unsigned int sequenceNumber){
    return this->makePacket(source, destination, source, PacketType::FANT, sequenceNumber, maxHopCount);
}

TestbedPacket* TestbedPacketFactory::makeBANT(const Packet *packet, unsigned int sequenceNumber){
    return this->makePacket(packet->getDestination(), packet->getSource(), packet->getDestination(), PacketType::BANT, sequenceNumber, maxHopCount);
}

TestbedPacket* TestbedPacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket, TestbedAddressPtr sender){
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), sender, PacketType::ACK, originalPacket->getSequenceNumber(), maxHopCount);
}

TESTBED_NAMESPACE_END
