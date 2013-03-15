/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_
#define ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_

#include <deque>
#include <cmath>
#include <ctime>
#include <numeric>
#include <stdlib.h>

#include "Packet.h"
#include "NextHop.h"
#include "RoutingTable.h"
#include "RoutingTableEntry.h"
#include "StochasticForwardingPolicy.h"

namespace ARA {
    /**
	 * The class provides the forwarding policy for the energy aware ant routing 
	 * (EARA).
	 *
	 */
    class EnergyAwareStochasticForwardingPolicy : public StochasticForwardingPolicy {
        public:
            EnergyAwareStochasticForwardingPolicy(){};
            virtual ~EnergyAwareStochasticForwardingPolicy(){};

            virtual NextHop* getNextHop(const Packet*);

            void setPheromoneWeight(float alpha);
            void setEnergyWeight(float beta);

        protected:
            /// the weight of the pheromone variable in the transmission probability
            float alpha;
            /// the weight of the energy variable in the transmission probability
            float beta;
    };

} /* namespace ARA */

#endif
