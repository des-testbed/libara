/*
 * $FU-Copyright$
 */

#include "omnetpp/PacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTPacket* PacketFactory::createOMNetPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, AddressPtr previousHop) {
    if(ttl == -1) {
        ttl = maxHopCount;
    }
    OMNeTPacket* packet = new OMNeTPacket(source, destination, sender, type, seqNr, ttl);
    if(previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }
    return packet;
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    return createOMNetPacket(source, destination, sender, type, seqNr, ttl, previousHop);
}

OMNETARA_NAMESPACE_END
