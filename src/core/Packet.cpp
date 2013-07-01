/*
 * $FU-Copyright$
 */

#include "Packet.h"
#include "PacketType.h"
#include <cstring>

typedef std::shared_ptr<ARA::Address> AddressPtr;

namespace ARA {

Packet::Packet(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) {
    this->source = source;
    this->destination = destination;
    this->sender = sender;
    this->previousHop = sender;
    this->type = type;
    this->seqNr = seqNr;

    if(payload != nullptr) {
        if(payloadSize == 0) {
            // assume this is a string and calculate the size
            payloadSize = std::strlen(payload)+1;
        }
        char* tmpPayload = new char[payloadSize];
        memcpy(tmpPayload, payload, payloadSize);
        this->payload = tmpPayload;
    }
    else {
        this->payload = nullptr;
    }

    this->payloadSize = payloadSize;
    this->ttl = ttl;
}

Packet::~Packet() {
   if(payload != nullptr) {
        delete[] payload;
   }
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

AddressPtr Packet::getPreviousHop() const {
    return previousHop;
}

char Packet::getType() const {
    return type;
}

unsigned int Packet::getSequenceNumber() const {
    return seqNr;
}

unsigned int Packet::getTTL() const {
    return ttl;
}

const char* Packet::getPayload() const {
    return payload;
}

void** Packet::getPayloadPointer() const {
    return (void**)&payload;
}

unsigned int Packet::getPayloadLength() const {
    return payloadSize;
}

Packet* Packet::setSender(AddressPtr newSender) {
    sender = newSender;
    return this;
}

Packet* Packet::setPreviousHop(AddressPtr newPreviousHop) {
    previousHop = newPreviousHop;
    return this;
}

void Packet::increaseTTL() {
    ttl++;
}

void Packet::decreaseTTL() {
    ttl--;
}

bool Packet::equals(const Packet* otherPacket) const {
    return this->seqNr == otherPacket->getSequenceNumber() && this->source->equals(otherPacket->getSource());
}

size_t Packet::getHashValue() const {
    // TODO Review and tweak this hash value generation (naiv implementation)
    size_t sourceHash = source->getHashValue();
    return sourceHash + seqNr;
}

} /* namespace ARA */
