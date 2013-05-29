/*
 * $FU-Copyright$
 */

#include "omnetpp/PacketFactory.h"
#include "omnetpp/OMNeTPacket.h"

OMNETARA_NAMESPACE_BEGIN

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    // the payload is simply ignored because this is done via packet encapsulation in OMNeT++
    OMNeTPacket* packet = new OMNeTPacket(source, destination, sender, type, seqNr, ttl);
    if(previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }
    return packet;
}

OMNETARA_NAMESPACE_END
