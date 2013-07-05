/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_ROUTINGTABLE_H_
#define ENERGY_AWARE_ROUTINGTABLE_H_

#include "ARAMacros.h"
#include "RoutingTable.h"
#include "EARARoutingTableEntry.h"

#include <unordered_map>

ARA_NAMESPACE_BEGIN

/**
 * This stores the normalized energy values for each route
 */
typedef std::unordered_map<AddressPtr, float, AddressHash, AddressPredicate> RouteEnergyFitnessMap;

class EnergyAwareRoutingTable : public RoutingTable {
    public:
        void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue);
        void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, float normalizedEnergyValue);

        /**
         * Updates only the energy value of a given hop but not the pheromone value.
         * If the route does not yet exist this method will do nothing.
         * @return true if the energy of an existing routing table entry has been updated, false otherwise
         */
        bool updateEnergyValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float newEnergyValue);

        /**
         * Get the energy value for a specific route in this routing table.
         */
        float getEnergyValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

    protected:
        void updateExistingEntry(RoutingTableEntry* oldEntry, RoutingTableEntry* newEntry);
        EARARoutingTableEntry* getRoutingTableEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

    protected:
        RouteEnergyFitnessMap routeEnergyFitnessValues;
};

ARA_NAMESPACE_END

#endif /* ROUTINGTABLE_H_ */
