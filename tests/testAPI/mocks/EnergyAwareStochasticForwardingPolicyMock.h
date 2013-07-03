/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_MOCK_H_
#define ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_MOCK_H_

#include "EnergyAwareStochasticForwardingPolicy.h"
#include "EnergyAwareRoutingTable.h"

ARA_NAMESPACE_BEGIN

/**
 * The EnergyAwareStochasticForwardingPolicyMock allows to set the seed of the random
 * number generator to a constant (and thus make the results predictable).
 */
class EnergyAwareStochasticForwardingPolicyMock : public EnergyAwareStochasticForwardingPolicy {
    public:
        EnergyAwareStochasticForwardingPolicyMock(EnergyAwareRoutingTable* routingTable, unsigned int seed);
};

ARA_NAMESPACE_END

#endif
