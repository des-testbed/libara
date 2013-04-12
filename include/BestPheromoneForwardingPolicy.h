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

class BestPheromoneForwardingPolicy : public ForwardingPolicy {
    public:
        NextHop* getNextHop(const Packet*, RoutingTable* routingTable);
};

ARA_NAMESPACE_END

#endif
