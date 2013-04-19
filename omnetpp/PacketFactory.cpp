/*
 * $FU-Copyright$
 */

#include "omnetpp/PacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTPacket* PacketFactory::createOMNetPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, AddressPtr penultimateHop) {
    if(ttl == -1) {
        ttl = maxHopCount;
    }
    OMNeTPacket* packet = new OMNeTPacket(source, destination, sender, type, seqNr, ttl);
    packet->setPenultimateHop(penultimateHop);
    return packet;
}

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr penultimateHop) {
    return createOMNetPacket(source, destination, sender, type, seqNr, ttl, penultimateHop);
}

OMNETARA_NAMESPACE_END
