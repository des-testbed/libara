/*
 * $FU-Copyright$
 */

#ifndef BEST_FORWARDING_POLICY_H_
#define BEST_FORWARDING_POLICY_H_

#include "ForwardingPolicy.h"
#include "NextHop.h"
#include "Packet.h"
#include "RoutingTable.h"

namespace ARA { 
    class BestPheromoneForwardingPolicy : public ForwardingPolicy {
        public:
            NextHop* getNextHop(const Packet*, RoutingTable* routingTable);
    };

} /* namespace ARA */

#endif
