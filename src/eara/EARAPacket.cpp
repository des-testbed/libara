/*
 * $FU-Copyright$
 */

#include "EARAPacket.h"

#include <climits>

ARA_NAMESPACE_BEGIN

EARAPacket::EARAPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize)
    : Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize) {
    totalEnergyValue = 0;
    minEnergyValue = UINT_MAX;
}

unsigned int EARAPacket::getTotalEnergyValue() const {
    return totalEnergyValue;
}

unsigned int EARAPacket::getMinimumEnergyValue() const {
    return minEnergyValue;
}

void EARAPacket::addEnergyValue(unsigned int newEnergyValue) {
    if (newEnergyValue < minEnergyValue) {
        minEnergyValue = newEnergyValue;
    }

    // protect against overflow
    if (UINT_MAX - totalEnergyValue >= newEnergyValue) {
        totalEnergyValue += newEnergyValue;
    }
}

void EARAPacket::setTotalEnergyValue(unsigned int energyValue) {
    totalEnergyValue = energyValue;
}

void EARAPacket::setMinimumEnergyValue(unsigned int energyValue) {
    minEnergyValue = energyValue;
}

ARA_NAMESPACE_END
