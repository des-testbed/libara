/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_PACKET_FACTORY_H_
#define OMNET_EARA_PACKET_FACTORY_H_

#include "OMNeTARAMacros.h"
#include "../EARAPacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * This class overrides the basic ARA::PacketFactory to let it produce OMNeTEARAPacket.
 */
class EARAPacketFactory : public ::ARA::EARAPacketFactory {
    public:
        EARAPacketFactory(int maxHopCount=20) : ::ARA::EARAPacketFactory(maxHopCount) {};
        void setMaxHopCount(int newMaxHopCount);

    protected:
        virtual EARAPacket* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);

    private:
        int calculatePacketSize(Packet* packet);
};

OMNETARA_NAMESPACE_END

#endif // OMNET_EARA_PACKET_FACTORY_H_
