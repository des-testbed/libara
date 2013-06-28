/*
 * $FU-Copyright$
 */

#ifndef EARA_PACKET_H_
#define EARA_PACKET_H_

#include "ARAMacros.h"
#include "Packet.h"

ARA_NAMESPACE_BEGIN

/**
 * EARAPackets extend the regular Packet class by some energy values the packet can store.
 */
class EARAPacket : public Packet {
    public:
        EARAPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0);
        unsigned int getTotalEnergyValue() const;
        unsigned int getMinimumEnergyValue() const;

        void addEnergyValue(unsigned int energyValue);

    protected:
        unsigned int totalEnergyValue;
        unsigned int minEnergyValue;
};

ARA_NAMESPACE_END

#endif // EARA_PACKET_H_
