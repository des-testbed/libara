/*
 * $FU-Copyright$
 */

#include "PacketFactory.h"
#include <cstring>

namespace ARA {

Packet* PacketFactory::makeClone(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getTTL(), originalPacket->getPayload(), originalPacket->getPayloadLength());
}

Packet* PacketFactory::makeFANT(const Packet* originalPacket, unsigned int newSequenceNumber, int maxHopCount) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::FANT, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeBANT(const Packet* originalPacket, unsigned int newSequenceNumber, int maxHopCount) {
    return makePacket(originalPacket->getDestination(), originalPacket->getSource(), originalPacket->getSender(), PacketType::BANT, newSequenceNumber, maxHopCount);
}

Packet* PacketFactory::makeDulicateWarningPacket(const Packet* originalPacket, unsigned int newSequenceNumber) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::DUPLICATE_ERROR, newSequenceNumber, 1, nullptr, 0);
}

Packet* PacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ACK, originalPacket->getSequenceNumber(), 1, nullptr, 0);
}

Packet* PacketFactory::makeRouteFailurePacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ROUTE_FAILURE, originalPacket->getSequenceNumber(), 1, nullptr, 0);
}

Packet* PacketFactory::makeEnergyDisseminationPacket(AddressPtr source, unsigned int seqNr, unsigned char energyLevel) {
    char payload[1];
    payload[0] = energyLevel;
    // FIXME the destination is set to source because we do not use or care for this field. However this seems not like a good idea
    return makePacket(source, source, source, PacketType::ENERGY_INFO, seqNr, 1, payload, 1);
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) {
    return new Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
}

} /* namespace ARA */
