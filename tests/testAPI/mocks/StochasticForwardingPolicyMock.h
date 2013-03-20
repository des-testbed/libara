/*
 * $FU-Copyright$
 */

#ifndef STOCHASTIC_FORWARDING_POLICY_MOCK_H_
#define STOCHASTIC_FORWARDING_POLICY_MOCK_H_

#include "StochasticForwardingPolicy.h"

namespace ARA {
    class StochasticForwardingPolicyMock : public StochasticForwardingPolicy {
        public:
            StochasticForwardingPolicyMock(unsigned int seed);
    };
}

#endif
