/*
 * $FU-Copyright$
 */

#include "PacketFactory.h"

namespace ARA {

PacketFactory::PacketFactory(int maxHopCount) {
    this->maxHopCount = maxHopCount;
    this->isPreviousHopFeatureEnabled = true;
}

void PacketFactory::setPreviousHopFeature(bool isActivated) {
    this->isPreviousHopFeatureEnabled = isActivated;
}

Packet* PacketFactory::makeClone(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getTTL(), originalPacket->getPayload(), originalPacket->getPayloadLength(), originalPacket->getPreviousHop());
}

Packet* PacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, sequenceNumber, maxHopCount, payload, payloadSize);
}

Packet* PacketFactory::makeFANT(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber) {
    return makePacket(source, destination, source, PacketType::FANT, sequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeBANT(const Packet* originalPacket, unsigned int sequenceNumber) {
    return makePacket(originalPacket->getDestination(), originalPacket->getSource(), originalPacket->getDestination(), PacketType::BANT, sequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeDulicateWarningPacket(const Packet* originalPacket, AddressPtr senderOfDuplicateWarning, unsigned int sequenceNumber) {
    return makePacket(senderOfDuplicateWarning, originalPacket->getDestination(), senderOfDuplicateWarning, PacketType::DUPLICATE_ERROR, sequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket, AddressPtr sender) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), sender, PacketType::ACK, originalPacket->getSequenceNumber(), maxHopCount);
}

Packet* PacketFactory::makeRouteFailurePacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber) {
    return makePacket(source, destination, source, PacketType::ROUTE_FAILURE, sequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeHelloPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber) {
    return makePacket(source, destination, source, PacketType::HELLO, sequenceNumber, maxHopCount);
}

Packet* PacketFactory::makePANT(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber) {
    return makePacket(source, destination, source, PacketType::PANT, sequenceNumber, maxHopCount);
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    Packet* packet = new Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    if(previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }
    return packet;
}

void PacketFactory::setMaxHopCount(int n) {
    maxHopCount = n;
}

int PacketFactory::getMaximumNrOfHops() {
    return maxHopCount;
}

} /* namespace ARA */
