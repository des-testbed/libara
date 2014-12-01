/*
 * $FU-Copyright$
 */

#ifndef STOCHASTIC_FORWARDING_POLICY_MOCK_H_
#define STOCHASTIC_FORWARDING_POLICY_MOCK_H_

#include "StochasticForwardingPolicy.h"
#include "RoutingTable.h"

ARA_NAMESPACE_BEGIN

/**
 * The StochasticForwardingPolicyMock allows to set the seed of the random
 * number generator to a constant (and thus make the results predictable).
 */
class StochasticForwardingPolicyMock : public StochasticForwardingPolicy {
    public:
        StochasticForwardingPolicyMock(RoutingTable* routingTable);
        StochasticForwardingPolicyMock(RoutingTable* routingTable, unsigned int seed);
};

ARA_NAMESPACE_END

#endif
