/*
 * $FU-Copyright$
 */

#include "EARAPacketFactory.h"
#include "EARAPacket.h"

ARA_NAMESPACE_BEGIN

Packet* EARAPacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    Packet* packet = new EARAPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize);
    if(previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }
    return packet;
}

ARA_NAMESPACE_END
