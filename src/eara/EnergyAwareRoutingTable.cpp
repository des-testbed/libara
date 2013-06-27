/*
 * $FU-Copyright$
 */

#include "EnergyAwareRoutingTable.h"

namespace ARA {

    void EnergyAwareRoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, unsigned char energyValue) {
        RoutingTable::update(destination, nextHop, interface, pheromoneValue);
        updateEnergyOfNode(nextHop, energyValue);
    }

    void EnergyAwareRoutingTable::updateEnergyOfNode(AddressPtr nodeAddress, unsigned char newEnergyLevel) {
        neighborEnergyValues[nodeAddress] = newEnergyLevel;
    }

    bool EnergyAwareRoutingTable::hasEnergyInformationFor(AddressPtr nodeAddress) {
        return neighborEnergyValues.find(nodeAddress) != neighborEnergyValues.end();
    }

    unsigned char EnergyAwareRoutingTable::getEnergyValueOf(AddressPtr nodeAddress) {
        triggerEvaporation(); //FIXME this should be done only in the ARAClient

        if (hasEnergyInformationFor(nodeAddress)) {
            return neighborEnergyValues[nodeAddress];
        }
        else {
            // FIXME maybe we should throw an exception instead
            return 0;
        }
    }
}
