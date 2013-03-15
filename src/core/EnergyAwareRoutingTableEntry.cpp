/*
 * $FU-Copyright$
 */

#include "EnergyAwareRoutingTableEntry.h"

namespace ARA {

EnergyAwareRoutingTableEntry::EnergyAwareRoutingTableEntry(std::shared_ptr<Address> address, NetworkInterface* interface, float pheromoneValue, float energyValue) : RoutingTableEntry(address, interface, pheromoneValue), energyValue(energyValue) {
}

float EnergyAwareRoutingTableEntry::getEnergyValue() {
    return energyValue;
}

void EnergyAwareRoutingTableEntry::setEnergyValue(float newEnergyValue) {
    this->energyValue = newEnergyValue;
}

} /* namespace ARA */
