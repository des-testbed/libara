/*
 * Packet.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: Friedrich Große
 */

#include "Packet.h"
#include "PacketType.h"

namespace ARA {

Packet::Packet(Address* source, Address* destination, Address* sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) {
    this->source = source;
    this->destination = destination;
    this->sender = sender;
    this->type = type;
    this->seqNr = seqNr;
    this->payload = payload;
    this->payloadSize = payloadSize;
    this->hopCount = hopCount;
}

Packet::~Packet() {
    if(this->sender != this->source) {
        delete this->sender;
    }
    delete this->source;
    delete this->destination;
}

Address* Packet::getSource() {
    return source;
}

Address* Packet::getDestination() {
    return destination;
}

Address* Packet::getSender() {
    return sender;
}

char Packet::getType() {
    return type;
}

unsigned int Packet::getSequenceNumber() {
    return seqNr;
}

unsigned int Packet::getHopCount() {
    return hopCount;
}

const char* Packet::getPayload() {
    return payload;
}

unsigned int Packet::getPayloadLength() {
    return payloadSize;
}

void Packet::setHopCount(unsigned int newValue) {
    hopCount = newValue;
}

bool Packet::equals(Packet* otherPacket) {
    return this->seqNr == otherPacket->getSequenceNumber() && this->source->equals(otherPacket->getSource());
}

Packet* Packet::clone() {
    return new Packet(source->clone(), destination->clone(), sender->clone(), type, seqNr, payload, payloadSize, hopCount);
}

Packet* Packet::createFANT(unsigned int sequenceNumber) {
    Packet* fant = new Packet(source->clone(), destination->clone(), sender->clone(), PacketType::FANT, sequenceNumber);
    fant->setHopCount(this->hopCount);
    return fant;
}

} /* namespace ARA */
