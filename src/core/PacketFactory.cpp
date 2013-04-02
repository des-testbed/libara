/*
 * $FU-Copyright$
 */

#include "PacketFactory.h"
#include <cstring>

namespace ARA {

Packet* PacketFactory::makeClone(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), originalPacket->getType(), originalPacket->getSequenceNumber(), originalPacket->getPayload(), originalPacket->getPayloadLength(), originalPacket->getHopCount());
}

Packet* PacketFactory::makeFANT(const Packet* originalPacket, unsigned int newSequenceNumber) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::FANT, newSequenceNumber);
}

Packet* PacketFactory::makeBANT(const Packet* originalPacket, unsigned int newSequenceNumber) {
    return makePacket(originalPacket->getDestination(), originalPacket->getSource(), originalPacket->getSender(), PacketType::BANT, newSequenceNumber);
}

Packet* PacketFactory::makeDulicateWarningPacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::DUPLICATE_ERROR, originalPacket->getSequenceNumber(), nullptr, 0, 1);
}

Packet* PacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ACK, originalPacket->getSequenceNumber(), nullptr, 0, 1);
}

Packet* PacketFactory::makeRouteFailurePacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ROUTE_FAILURE, originalPacket->getSequenceNumber(), nullptr, 0, 1);
}

Packet* PacketFactory::makeEnergyDisseminationPacket(AddressPtr source, unsigned int seqNr, unsigned char energyLevel) {
    char payload[1];
    payload[0] = energyLevel;
    // FIXME the destination is set to source because we do not use or care for this field. However this seems not like a good idea
    return makePacket(source, source, source, PacketType::ENERGY_INFO, seqNr, payload, 1, 1);
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) {
    return new Packet(source, destination, sender, type, seqNr, payload, payloadSize, hopCount);
}

} /* namespace ARA */
