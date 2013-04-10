/*
 * $FU-Copyright$
 */

#include "omnetpp/PacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTPacket* PacketFactory::createOMNetPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl) {
    if(ttl == -1) {
        ttl = maxHopCount;
    }
    return new OMNeTPacket(source, destination, sender, type, seqNr, ttl);
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) {
    return createOMNetPacket(source, destination, sender, type, seqNr, ttl);
}

OMNETARA_NAMESPACE_END
