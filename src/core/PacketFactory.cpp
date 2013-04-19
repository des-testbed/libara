/*
 * $FU-Copyright$
 */

#include "PacketFactory.h"

namespace ARA {

PacketFactory::PacketFactory(int maxHopCount) {
    this->maxHopCount = maxHopCount;
}

Packet* PacketFactory::makeClone(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getTTL(), originalPacket->getPayload(), originalPacket->getPayloadLength(), originalPacket->getPenultimateHop());
}

Packet* PacketFactory::makeFANT(AddressPtr source, AddressPtr destination, unsigned int newSequenceNumber) {
    return makePacket(source, destination, source, PacketType::FANT, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeBANT(const Packet* originalPacket, unsigned int newSequenceNumber) {
    return makePacket(originalPacket->getDestination(), originalPacket->getSource(), originalPacket->getSender(), PacketType::BANT, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeDulicateWarningPacket(const Packet* originalPacket, AddressPtr senderOfDuplicateWarning, unsigned int newSequenceNumber) {
    return makePacket(senderOfDuplicateWarning, originalPacket->getDestination(), senderOfDuplicateWarning, PacketType::DUPLICATE_ERROR, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ACK, originalPacket->getSequenceNumber(), maxHopCount);
}

Packet* PacketFactory::makeRouteFailurePacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ROUTE_FAILURE, originalPacket->getSequenceNumber(), maxHopCount);
}

Packet* PacketFactory::makeEnergyDisseminationPacket(AddressPtr source, unsigned int seqNr, unsigned char energyLevel) {
    char payload[1];
    payload[0] = energyLevel;
    // FIXME the destination is set to source because we do not use or care for this field. However this seems not like a good idea
    return makePacket(source, source, source, PacketType::ENERGY_INFO, seqNr, maxHopCount, payload, 1);
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr penultimateHop) {
    Packet* packet = new Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    packet->setPenultimateHop(penultimateHop);
    return packet;
}

void PacketFactory::setMaxHopCount(int n) {
    maxHopCount = n;
}

int PacketFactory::getMaximumNrOfHops() {
    return maxHopCount;
}

} /* namespace ARA */
