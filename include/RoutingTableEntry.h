/*
 * $FU-Copyright$
 */

#ifndef ROUTINGTABLEENTRY_H_
#define ROUTINGTABLEENTRY_H_

#include "ARAMacros.h"
#include "NextHop.h"
#include "NetworkInterface.h"

#include <iostream>

ARA_NAMESPACE_BEGIN

class RoutingTableEntry {
    public:
        RoutingTableEntry(std::shared_ptr<Address> address, NetworkInterface* interface, float pheromoneValue);
        ~RoutingTableEntry();

        NextHop* getNextHop();
        std::shared_ptr<Address> getAddress();
        NetworkInterface* getNetworkInterface();
        float getPheromoneValue();
        void setPheromoneValue(float newPheromoneValue);

        friend std::ostream& operator<<(std::ostream& out, RoutingTableEntry& entry) {
            out << "[next hop] " << entry.getAddress()->toString() << " [phi] " << entry.getPheromoneValue();
            return out;
        }

    private:
        NextHop* nextHop;
        float pheromoneValue;
};

ARA_NAMESPACE_END

#endif /* ROUTINGTABLEENTRY_H_ */
