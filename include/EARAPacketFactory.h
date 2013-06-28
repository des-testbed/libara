/*
 * $FU-Copyright$
 */

#ifndef EARA_PACKET_FACTORY_H_
#define EARA_PACKET_FACTORY_H_

#include "ARAMacros.h"
#include "PacketFactory.h"

ARA_NAMESPACE_BEGIN

/**
 * The PacketFactory is responsible for creating the packet instances for an ~AbstractARAClient.
 * This class is necessary so we can decide at runtime what the concrete Packet class shall be.
 * If you do not want to use another Packet implementation class than the standard implementation
 * you need to override PacketFactory::makePacket(...) to return something that inherits from packet.
 */
class EARAPacketFactory : public PacketFactory {
    public:
        EARAPacketFactory(int maxHopCount) : PacketFactory(maxHopCount) {};

    protected:

        /**
          * This will always create EARAPacket instances
          */
         virtual Packet* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);

};

ARA_NAMESPACE_END

#endif // PACKET_FACTORY_H_
