/*
 * $FU-Copyright$
 */

#include "Packet.h"
#include "PacketType.h"
#include <cstring>

typedef std::shared_ptr<ARA::Address> AddressPtr;

namespace ARA {

Packet::Packet(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) {
    this->source = source;
    this->destination = destination;
    this->sender = sender;
    this->type = type;
    this->seqNr = seqNr;

    if(payload != NULL && payloadSize == 0) {
        payloadSize = std::strlen(payload);
    }

    this->payload = payload;
    this->payloadSize = payloadSize;
    this->hopCount = hopCount;
}

Packet::Packet(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, unsigned int hopCount) {
    this->source = source;
    this->destination = destination;
    this->sender = sender;
    this->type = type;
    this->seqNr = seqNr;
    this->hopCount = hopCount;

    this->payload = NULL;
    this->payloadSize = 0;
}

Packet::Packet(AddressPtr source, AddressPtr destination, char type, unsigned int seqNr) {
    this->source = source;
    this->destination = destination;
    this->sender = source;
    this->type = type;
    this->seqNr = seqNr;

    this->hopCount = 1;
    this->payload = NULL;
    this->payloadSize = 0;
}

Packet::~Packet() {
    // Address cleanup is done by the shared_ptrs
}

AddressPtr Packet::getSource() const {
    return source;
}

AddressPtr Packet::getDestination() const {
    return destination;
}

AddressPtr Packet::getSender() const {
    return sender;
}

char Packet::getType() const {
    return type;
}

unsigned int Packet::getSequenceNumber() const {
    return seqNr;
}

unsigned int Packet::getHopCount() const {
    return hopCount;
}

const char* Packet::getPayload() const {
    return payload;
}

unsigned int Packet::getPayloadLength() const {
    return payloadSize;
}

void Packet::setHopCount(unsigned int newValue) {
    hopCount = newValue;
}

void Packet::increaseHopCount() {
    hopCount++;
}

void Packet::decreaseHopCount() {
    hopCount--;
}

void Packet::setSender(AddressPtr newSender) {
    sender = newSender;
}

bool Packet::equals(const Packet* otherPacket) const {
    return this->seqNr == otherPacket->getSequenceNumber() && this->source->equals(otherPacket->getSource());
}

Packet* Packet::clone() const {
    return new Packet(source, destination, sender, type, seqNr, payload, payloadSize, hopCount);
}

Packet* Packet::createFANT(unsigned int sequenceNumber) const {
    return new Packet(source, destination, sender, PacketType::FANT, sequenceNumber);
}

Packet* Packet::createBANT(unsigned int sequenceNumber) const {
    return new Packet(destination, source, sender, PacketType::BANT, sequenceNumber);
}

Packet* Packet::createDuplicateWarning() const {
    return new Packet(source, destination, sender, PacketType::DUPLICATE_ERROR, seqNr, 1);
}

Packet* Packet::createAcknowledgment() const {
    return new Packet(source, destination, sender, PacketType::ACK, seqNr, 1);
}

Packet* Packet::createRouteFailurePacket() const {
    return new Packet(source, destination, sender, PacketType::ROUTE_FAILURE, seqNr, 1);
}

size_t Packet::getHashValue() const {
    // TODO Review and tweak this hash value generation (naiv implementation)
    size_t sourceHash = source->getHashValue();
    return sourceHash + seqNr;
}

} /* namespace ARA */
