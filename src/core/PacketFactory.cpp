/*
 * $FU-Copyright$
 */

#include "PacketFactory.h"

typedef std::shared_ptr<ARA::Address> AddressPtr;

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
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::DUPLICATE_ERROR, originalPacket->getSequenceNumber(), NULL, 0, 1);
}

Packet* PacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ACK, originalPacket->getSequenceNumber(), NULL, 0, 1);
}

Packet* PacketFactory::makeRouteFailurePacket(const Packet* originalPacket) {
    return makePacket(originalPacket->getSource(), originalPacket->getDestination(), originalPacket->getSender(), PacketType::ROUTE_FAILURE, originalPacket->getSequenceNumber(), NULL, 0, 1);
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) {
    return new Packet(source, destination, sender, type, seqNr, payload, payloadSize, hopCount);

}

} /* namespace ARA */
