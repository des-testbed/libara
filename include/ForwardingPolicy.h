/*
 * $FU-Copyright$
 */

#ifndef FORWARDING_POLICY_H_
#define FORWARDING_POLICY_H_

#include "ARAMacros.h"
#include "NextHop.h"
#include "Packet.h"

ARA_NAMESPACE_BEGIN

/**
 * This purely virtual interface is used by the AbstractARAClient to determine
 * the next hop for a given packet.
 */
class ForwardingPolicy {
    public:
        virtual ~ForwardingPolicy() {};

        /**
         * Return the NextHop for the given packet according to this packet forwarding
         * policy.
         */
        virtual NextHop* getNextHop(const Packet* packet) = 0;
};

ARA_NAMESPACE_END

#endif
