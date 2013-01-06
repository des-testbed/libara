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
    Packet* fant = new Packet(source->clone(), destination->clone(), sender->clone(), PacketType::FANT, sequenceNumber);
    fant->setHopCount(this->hopCount);
    return fant;
}

} /* namespace ARA */
