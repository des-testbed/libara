/*
 * $FU-Copyright$
 */

#include "omnetpp/PacketFactory.h"
#include "omnetpp/OMNeTPacket.h"

OMNETARA_NAMESPACE_BEGIN

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) {
    return new OMNeTPacket(source, destination, sender, type, seqNr, ttl);
}

OMNETARA_NAMESPACE_END
