/*
 * $FU-Copyright$
 */

#include "EARARoutingTableEntry.h"
#include "Exception.h"

ARA_NAMESPACE_BEGIN

EARARoutingTableEntry::EARARoutingTableEntry(AddressPtr address, NetworkInterface* interface, float pheromoneValue, float energyValue)
    : RoutingTableEntry(address, interface, pheromoneValue) {

    checkIsNormalized(energyValue);
    this->energyValue = energyValue;
}

void EARARoutingTableEntry::checkIsNormalized(float energyValue) const {
    if(energyValue < 1 || energyValue > 10) {
        throw Exception("Normalized energy values must be between 1 and 10");
    }
}

float EARARoutingTableEntry::getEnergyValue() {
    return energyValue;
}

void EARARoutingTableEntry::setEnergyValue(float newEnergyValue) {
    checkIsNormalized(newEnergyValue);
    this->energyValue = newEnergyValue;
}


ARA_NAMESPACE_END
