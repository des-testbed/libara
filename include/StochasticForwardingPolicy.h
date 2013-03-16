/*
 * $FU-Copyright$
 */

#ifndef STOCHASTIC_FORWARDING_POLICY_H_
#define STOCHASTIC_FORWARDING_POLICY_H_

#include "ForwardingPolicy.h"
#include "NextHop.h"
#include "Packet.h"
#include "RoutingTable.h"

namespace ARA {
    class StochasticForwardingPolicy : public ForwardingPolicy {
        public:
            NextHop* getNextHop(const Packet*, RoutingTable* routingTable);

        protected:
            void initializeRandomNumberGenerator();
            float getRandomNumber();
    };

} /* namespace ARA */

#endif
