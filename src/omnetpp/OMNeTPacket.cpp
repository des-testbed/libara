/*
 * OMNeTPacket.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: Friedrich Große
 */

#include "OMNeTPacket.h"

namespace ARA {

OMNeTPacket::OMNeTPacket(OMNeTAddress* source, OMNeTAddress* destination, unsigned int type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) {
    this->source = source;
    this->destination = destination;
    this->type = type;
    this->seqNr = seqNr;
    this->payload = payload;
    this->payloadSize = payloadSize;
    this->hopCount = hopCount;
}

Address* OMNeTPacket::getSource() {
    return source;
}

Address* OMNeTPacket::getDestination() {
    return destination;
}

unsigned int OMNeTPacket::getType() {
    return type;
}

unsigned int OMNeTPacket::getSequenceNumber() {
    return seqNr;
}

unsigned int OMNeTPacket::getHopCount() {
    return hopCount;
}

const char* OMNeTPacket::getPayload() {
    return payload;
}

unsigned int OMNeTPacket::getPayloadLength() {
    return payloadSize;
}

} /* namespace ARA */
