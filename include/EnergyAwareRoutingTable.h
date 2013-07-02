/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_ROUTINGTABLE_H_
#define ENERGY_AWARE_ROUTINGTABLE_H_

#include "ARAMacros.h"
#include "RoutingTable.h"
#include "Address.h"

#include <unordered_map>

ARA_NAMESPACE_BEGIN

/**
 * This stores the normalized energy values for each route
 */
typedef std::unordered_map<AddressPtr, float, AddressHash, AddressPredicate> RouteEnergyFitnessMap;

class EnergyAwareRoutingTable : public RoutingTable {
    public:
        void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, float normalizedEnergyValue);

        /**
         * Get the energy value for a specific route in this routing table.
         */
        float getEnergyValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

    protected:
        RouteEnergyFitnessMap routeEnergyFitnessValues;
};

ARA_NAMESPACE_END

#endif /* ROUTINGTABLE_H_ */
