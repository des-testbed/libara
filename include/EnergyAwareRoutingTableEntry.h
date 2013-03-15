/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_ROUTINGTABLEENTRY_H_
#define ENERGY_AWARE_ROUTINGTABLEENTRY_H_

#include <memory>
#include "NextHop.h"
#include "Address.h"
#include "NetworkInterface.h"
#include "RoutingTableEntry.h"

namespace ARA {
    class EnergyAwareRoutingTableEntry : public RoutingTableEntry {
        public:
            EnergyAwareRoutingTableEntry(std::shared_ptr<Address> address, NetworkInterface* interface, float pheromoneValue, float energyValue);
            ~EnergyAwareRoutingTableEntry();

            float getEnergyValue();
            void setEnergyValue(float newEnergyValue);

        private:
            float energyValue;
};

} /* namespace ARA */
#endif /* ENERGY_AWARE_ROUTINGTABLEENTRY_H_ */
