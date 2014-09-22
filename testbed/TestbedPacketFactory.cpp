/*
 * $FU-Copyright$
 */

#include "TestbedPacketFactory.h"

extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;

TESTBED_NAMESPACE_BEGIN

TestbedPacketFactory::TestbedPacketFactory(int maxHopCount) : PacketFactory(maxHopCount) {}

TestbedPacket* TestbedPacketFactory::makePacket(dessert_msg_t* message) {
    /// the destination address (if it is set)
    ara_address_t address;

    /// obtain the ethernet header
    dessert_ext_t* extension = nullptr;
    dessert_msg_getext(message, &extension, DESSERT_EXT_ETH, 0);
    ether_header* ethernetFrame = (ether_header*) extension->data;

    char packetType = message->u8;
    unsigned int sequenceNumber = ntohs(message->u16);
    int ttl = message->ttl;

    // TODO: check if that always make sense
    // determine the sender of the packet
    TestbedAddressPtr sender = std::make_shared<TestbedAddress>(message->l2h.ether_shost);

    TestbedAddressPtr destination;
    /// TODO: That's a bit complicated, let's make that easier
    if ((packetType == PacketType::FANT) || (packetType == PacketType::BANT)) {
        // get the destination address from the ant agent
        if (packetType == PacketType::FANT) {
            if (dessert_msg_getext(message, &extension, ARA_EXT_FANT, 0) > 0) {
                std::memcpy(address, extension->data, sizeof(ara_address_t));
            }
        } else {
            if (dessert_msg_getext(message, &extension, ARA_EXT_BANT, 0) > 0) {
                std::memcpy(address, extension->data, sizeof(ara_address_t));
            }
        }

        destination = std::make_shared<TestbedAddress>(address);
        // DEBUG: std::cerr << "[TestbedPacketFactory::makePaket] the destination address of the ant agent ist " << destination->toString() << std::endl;
    } else {
        destination = std::make_shared<TestbedAddress>(ethernetFrame->ether_dhost);
    }

    TestbedAddressPtr source = std::make_shared<TestbedAddress>(ethernetFrame->ether_shost);

    TestbedPacket* packet = new TestbedPacket(source, destination, sender, packetType, sequenceNumber, ttl);

    if (ntohs(message->plen) > 0) {
        packet->setPayloadType(ethernetFrame->ether_type);
        packet->addPayload(message);
    }

    return packet;
}

RoutingExtension* TestbedPacketFactory::getRoutingExtension(dessert_msg_t* message) {
    dessert_ext_t* extension = nullptr;

    if (dessert_msg_getext(message, &extension, DESSERT_EXT_USER, 0) == 0) {
        return nullptr;
    }

    return (RoutingExtension*)extension->data;
}

TestbedPacket* TestbedPacketFactory::makeNewPacket(dessert_msg_t* message) {
    return this->makePacket(message);
}

TestbedPacket* TestbedPacketFactory::makeClone(const TestbedPacket* packet) {
    return makePacket(packet->getSource(), packet->getDestination(), packet->getSender(), packet->getType(), packet->getSequenceNumber(), packet->getTTL(), packet->getPayload(), packet->getPayloadLength(), packet->getPreviousHop());
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, dessert_msg_t* payload, unsigned int payloadSize) {
    TestbedPacket* packet = this->makePacket(source, destination, source, PacketType::DATA, sequenceNumber, maxHopCount);
    packet->addPayload(payload);
    return packet;
}

TestbedPacket* TestbedPacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, sequenceNumber, this->maxHopCount, (const char*)payload, payloadSize);
}

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


bool TestbedPacketFactory::checkDessertMessage(dessert_msg_t* message){
    int result = -4;
    /// determine the expected message length (header + payload)
    int expectedMessageSize = ntohs(message->hlen) + ntohs(message->plen);

    if ((result = dessert_msg_check(message, expectedMessageSize)) == DESSERT_OK) {
        return true;
    } else if(result == -1) {
        // DEBUG: 
        std::cerr << "[TestbedPacketFactory::checkDessertMessage] the message was too large for the buffer" << std::endl;
    } else if(result == -2) {
        // DEBUG: 
        std::cerr << "[TestbedPacketFactory::checkDessertMessage] the message was not intended for this daemon" << std::endl;
    } else if(result == -3) {
        // DEBUG: 
        std::cerr << "[TestbedPacketFactory::checkDessertMessage] an extension is not consistent" << std::endl;
    } else {
        // DEBUG: 
        std::cerr << "[TestbedPacketFactory::checkDessertMessage] unknown error code" << std::endl;
    }

    return false;
}


TESTBED_NAMESPACE_END
