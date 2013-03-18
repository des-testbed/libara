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
#include "EnergyAwareRoutingTableEntry.h"
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

<<<<<<< HEAD
            virtual NextHop* getNextHop(const Packet* packet, RoutingTable* routingTable);
=======
            virtual NextHop* getNextHop(const Packet*);
>>>>>>> 02b19ac7086df67a043277a98cb5532f9f7c08f9

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
