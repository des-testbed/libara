/*
 * $FU-Copyright$
 */

#ifndef EARA_ROUTINGTABLEENTRY_H_
#define EARA_ROUTINGTABLEENTRY_H_

#include "ARAMacros.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "NetworkInterface.h"

#include <iostream>

ARA_NAMESPACE_BEGIN

class EARARoutingTableEntry : public RoutingTableEntry {
    public:
        EARARoutingTableEntry(AddressPtr address, NetworkInterface* interface, float pheromoneValue, float normalizedEnergyValue);

        float getEnergyValue();
        void setEnergyValue(float newEnergyValue);

        friend std::ostream& operator<<(std::ostream& out, EARARoutingTableEntry& entry) {
            // we need to calculate the percent value from the normalized value
            float procentValue = ((entry.getEnergyValue() - 1) / 9.0) * 100;
            out << "[next hop] " << entry.getAddress()->toString() << " [phi] " << entry.getPheromoneValue() << " [energy] " << procentValue << "%";
            return out;
        }

    private:

        /**
         * Check that the given value is within the range [0, 1] (inclusive)
         */
        void checkIsNormalized(float energyValue) const;

        float energyValue;
};

ARA_NAMESPACE_END

#endif /* EARA_ROUTINGTABLEENTRY_H_ */
