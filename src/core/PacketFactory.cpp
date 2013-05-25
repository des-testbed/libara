/*
 * $FU-Copyright$
 */

#include "PacketFactory.h"

namespace ARA {

PacketFactory::PacketFactory(int maxHopCount) {
    this->maxHopCount = maxHopCount;
}

Packet* PacketFactory::makeClone(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getTTL(), originalPacket->getPayload(), originalPacket->getPayloadLength(), originalPacket->getPreviousHop());
}

Packet* PacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int newSequenceNumber, const char* payload, unsigned int payloadSize) {
    return makePacket(source, destination, source, PacketType::DATA, newSequenceNumber, maxHopCount, payload, payloadSize);
}

Packet* PacketFactory::makeFANT(AddressPtr source, AddressPtr destination, unsigned int newSequenceNumber) {
    return makePacket(source, destination, source, PacketType::FANT, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeBANT(const Packet* originalPacket, unsigned int newSequenceNumber) {
    return makePacket(originalPacket->getDestination(), originalPacket->getSource(), originalPacket->getDestination(), PacketType::BANT, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeDulicateWarningPacket(const Packet* originalPacket, AddressPtr senderOfDuplicateWarning, unsigned int newSequenceNumber) {
    return makePacket(senderOfDuplicateWarning, originalPacket->getDestination(), senderOfDuplicateWarning, PacketType::DUPLICATE_ERROR, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket, AddressPtr sender) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), sender, PacketType::ACK, originalPacket->getSequenceNumber(), maxHopCount);
}

Packet* PacketFactory::makeRouteFailurePacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber) {
    return makePacket(source, destination, source, PacketType::ROUTE_FAILURE, sequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeEnergyDisseminationPacket(AddressPtr source, unsigned int seqNr, unsigned char energyLevel) {
    char payload[1];
    payload[0] = energyLevel;
    // FIXME the destination is set to source because we do not use or care for this field. However this seems not like a good idea
    return makePacket(source, source, source, PacketType::ENERGY_INFO, seqNr, maxHopCount, payload, 1);
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
