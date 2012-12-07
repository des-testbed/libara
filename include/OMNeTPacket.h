/*
 * OMNeTPacket.h
 *
 *  Created on: Dec 2, 2012
 *      Author: Friedrich Große
 */

#ifndef OMNETPACKET_H_
#define OMNETPACKET_H_

#include "Packet.h"
#include "OMNeTAddress.h"

namespace ARA {

class OMNeTPacket {
public:
    //TODO We do not need all the size of an unsigned int to store the type (one byte would be sufficient)
    OMNeTPacket(OMNeTAddress* source, OMNeTAddress* destination, unsigned int type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount = 0);

    Address* getSource();
    Address* getDestination();
    unsigned int getType();
    unsigned int getSequenceNumber();
    unsigned int getHopCount();
    const char* getPayload();
    unsigned int getPayloadLength();

private:
    OMNeTAddress* source;
    OMNeTAddress* destination;
    unsigned int type;
    unsigned int seqNr;
    const char* payload;
    unsigned int payloadSize;
    unsigned int hopCount;
};

} /* namespace ARA */
#endif /* OMNETPACKET_H_ */
