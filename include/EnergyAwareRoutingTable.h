/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_ROUTINGTABLE_H_
#define ENERGY_AWARE_ROUTINGTABLE_H_

#include "ARAMacros.h"
#include "RoutingTable.h"
#include "EARARoutingTableEntry.h"

ARA_NAMESPACE_BEGIN

class EnergyAwareRoutingTable : public RoutingTable {
    public:
        virtual void update(AddressPtr destination, RoutingTableEntry* entry) {RoutingTable::update(destination, entry);};
        virtual void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue);
        virtual void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, float normalizedEnergyValue);

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
};

ARA_NAMESPACE_END

#endif /* ROUTINGTABLE_H_ */
