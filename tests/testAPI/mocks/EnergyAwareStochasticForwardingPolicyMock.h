/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_MOCK_H_
#define ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_MOCK_H_

#include "EnergyAwareStochasticForwardingPolicy.h"

namespace ARA {
    /**
     * The EnergyAwareStochasticForwardingPolicyMock allows to set the seed of the random
     * number generator to a constant (and thus make the results predictable).
     */
    class EnergyAwareStochasticForwardingPolicyMock : public EnergyAwareStochasticForwardingPolicy {
        public:
            EnergyAwareStochasticForwardingPolicyMock(unsigned int seed);
    };
}

#endif
