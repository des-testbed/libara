/*
 * $FU-Copyright$
 */

#include "omnetpp/PacketFactory.h"
#include "omnetpp/OMNeTPacket.h"

OMNETARA_NAMESPACE_BEGIN

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, const char* payload, unsigned int payloadSize, unsigned int hopCount) {
    return new OMNeTPacket(source, destination, sender, type, seqNr, hopCount, payload, payloadSize);
}

OMNETARA_NAMESPACE_END
