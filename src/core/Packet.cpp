/*
 * Packet.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: Friedrich Große
 */

#include "Packet.h"
#include "PacketType.h"
#include <cstring>

namespace ARA {

Packet::Packet(Address* source, Address* destination, Address* sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) {
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

Packet::Packet(Address* source, Address* destination, Address* sender, char type, unsigned int seqNr, unsigned int hopCount) {
    this->source = source;
    this->destination = destination;
    this->sender = sender;
    this->type = type;
    this->seqNr = seqNr;
    this->hopCount = hopCount;

    this->payload = NULL;
    this->payloadSize = 0;
}

Packet::Packet(Address* source, Address* destination, char type, unsigned int seqNr) {
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
    if(sender != source) {
        delete sender;
    }
    delete source;
    delete destination;
}

Address* Packet::getSource() const {
    return source;
}

Address* Packet::getDestination() const {
    return destination;
}

Address* Packet::getSender() const {
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

void Packet::setSender(Address* newSender) {
    if(sender != source) {
        delete sender;
    }
    sender = newSender;
}

bool Packet::equals(const Packet* otherPacket) const {
    return this->seqNr == otherPacket->getSequenceNumber() && this->source->equals(otherPacket->getSource());
}

Packet* Packet::clone() const {
    return new Packet(source->clone(), destination->clone(), sender->clone(), type, seqNr, payload, payloadSize, hopCount);
}

Packet* Packet::createFANT(unsigned int sequenceNumber) const {
    // TODO cloning the sender here has no real meaning. Couldn't we just set the sender to NULL? (beware of resulting segfaults in the destructor!!!)
    Packet* fant = new Packet(source->clone(), destination->clone(), sender->clone(), PacketType::FANT, sequenceNumber);
    fant->setHopCount(this->hopCount);
    return fant;
}

Packet* Packet::createBANT(unsigned int sequenceNumber) const {
    unsigned int hopCount = 0;
    Packet* bant = new Packet(destination->clone(), source->clone(), sender->clone(), PacketType::BANT, sequenceNumber, hopCount);
    return bant;
}

size_t Packet::getHashValue() const {
    // TODO Review and tweak this hash value generation (naiv implementation)
    size_t sourceHash = source->getHashValue();
    return sourceHash + seqNr;
}

} /* namespace ARA */
