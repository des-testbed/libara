/*
 * $FU-Copyright$
 */

#ifndef STOCHASTIC_FORWARDING_POLICY_H_
#define STOCHASTIC_FORWARDING_POLICY_H_

#include "ForwardingPolicy.h"
#include "NextHop.h"
#include "Packet.h"
#include "RoutingTable.h"

#include <deque>
#include <numeric>

namespace ARA {
    class StochasticForwardingPolicy : public ForwardingPolicy {
        public:
            /**
             * Stochastically selects the next hop according to their pheromone value
             * but never the sender of the given packet.
             */
            NextHop* getNextHop(const Packet*, RoutingTable* routingTable);

        protected:
            void initializeRandomNumberGenerator(unsigned int seed=((unsigned)time(0)));
            float getRandomNumber();
    };

} /* namespace ARA */

#endif
