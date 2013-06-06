/*
 * $FU-Copyright$
 */

#include "omnetpp/PacketFactory.h"
#include "omnetpp/OMNeTPacket.h"

OMNETARA_NAMESPACE_BEGIN

Packet* PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize, AddressPtr previousHop) {
    // the payload is simply ignored because this is done via packet encapsulation in OMNeT++
    OMNeTPacket* packet = new OMNeTPacket(source, destination, sender, type, seqNr, ttl);

    if(isPreviousHopFeatureEnabled && previousHop != nullptr) {
        packet->setPreviousHop(previousHop);
    }

    int size = calculatePacketSize(packet);
    packet->setBitLength(size);

    return packet;
}

int PacketFactory::calculatePacketSize(Packet* packet) {
    int totalSize = 0;

    // at first lets define how big one address is. We use IPv4
    int sizeOfOneAddress = 32;

    // the source address
    totalSize += sizeOfOneAddress;

    // the sender address
    totalSize += sizeOfOneAddress;

    // the destination address
    totalSize += sizeOfOneAddress;

    if (isPreviousHopFeatureEnabled) {
        // the previous hop address
        totalSize += sizeOfOneAddress;
    }

    // 4 bits for the type (can store 16 different types which is really enough)
    totalSize += 4;

    // sequence number (32 bit integer)
    totalSize += 32;

    // TTL (256 values will suffice)
    totalSize += 8;

    // should be 140 bits = 7.5 bytes
    return totalSize;
}

OMNETARA_NAMESPACE_END
