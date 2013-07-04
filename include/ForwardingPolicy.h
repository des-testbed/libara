/*
 * $FU-Copyright$
 */

#ifndef FORWARDING_POLICY_H_
#define FORWARDING_POLICY_H_

#include "ARAMacros.h"
#include "RoutingTable.h"
#include "NextHop.h"
#include "Packet.h"

ARA_NAMESPACE_BEGIN

/**
 * This purely virtual interface is used by the AbstractARAClient to determine
 * the next hop for a given packet.
 */
class ForwardingPolicy {
    public:
        /**
         * Creates a new forwarding policy and gives it a RoutingTable object
         * which will be used in the forwarding decision.
         */
        ForwardingPolicy(RoutingTable* routingTable);
        virtual ~ForwardingPolicy() {};

        /**
         * Return the NextHop for the given packet according to this packet forwarding
         * policy.
         */
        virtual NextHop* getNextHop(const Packet* packet) = 0;

    protected:
        RoutingTable* routingTable;
};

ARA_NAMESPACE_END

#endif
