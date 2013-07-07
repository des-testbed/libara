/*
 * $FU-Copyright$
 */

#ifndef EARA_FORWARDING_POLICY_H_
#define EARA_FORWARDING_POLICY_H_

#include "ARAMacros.h"
#include "ForwardingPolicy.h"
#include "RoutingTable.h"
#include "NextHop.h"
#include "Packet.h"

ARA_NAMESPACE_BEGIN

/**
 * This purely virtual interface is used by the AbstractEARAClient to determine
 * the next hop for a given packet and calculate the initial energy fitness with the given weights.
 */
class EARAForwardingPolicy : public ForwardingPolicy {
    public:
        virtual float getPheromoneWeight() = 0;
        virtual float getEnergyWeight() = 0;
};

ARA_NAMESPACE_END

#endif
