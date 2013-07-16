/*
 * $FU-Copyright$
 */

#ifndef BEST_FORWARDING_POLICY_H_
#define BEST_FORWARDING_POLICY_H_

#include "ARAMacros.h"
#include "ForwardingPolicy.h"
#include "NextHop.h"
#include "Packet.h"
#include "RoutingTable.h"

ARA_NAMESPACE_BEGIN

class BestPheromoneForwardingPolicy : public virtual ForwardingPolicy {
    public:
        BestPheromoneForwardingPolicy(RoutingTable* routingTable);

        /**
         * Returns the next hop with the highest corresponding pheromone value.
         */
        NextHop* getNextHop(const Packet*);

    protected:
            RoutingTable* routingTable;
};

ARA_NAMESPACE_END

#endif
