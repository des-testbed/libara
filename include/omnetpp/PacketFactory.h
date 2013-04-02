/*
 * $FU-Copyright$
 */

#ifndef OMNET_PACKET_FACTORY_H_
#define OMNET_PACKET_FACTORY_H_

#include "OMNeTARAMacros.h"
#include "../PacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * This class overrides the basic ARA::PacketFactory to let it produce
 * OMneTPackets.
 */
class PacketFactory : public ::ARA::PacketFactory {
protected:
    Packet* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, const char* payload=nullptr, unsigned int payloadSize=0, unsigned int hopCount = 0);
};

OMNETARA_NAMESPACE_END

#endif // OMNET_PACKET_FACTORY_H_
