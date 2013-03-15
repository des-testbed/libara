/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_ROUTINGTABLE_H_
#define ENERGY_AWARE_ROUTINGTABLE_H_

#include "Packet.h"
#include "Address.h"
#include "NetworkInterface.h"
#include "RoutingTable.h"
#include "EnergyAwareRoutingTableEntry.h"

#include <deque>
#include <unordered_map>

namespace ARA {
    class EnergyAwareRoutingTable : public RoutingTable {
        public:
           float getEnergyValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
           void update(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface, float pheromoneValue, float energyValue);

        protected:
           void updateExistingEntry(RoutingTableEntry *oldEntry, RoutingTableEntry *newEntry);
};

} /* namespace ARA */
#endif /* ROUTINGTABLE_H_ */
