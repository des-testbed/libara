/*
 * $FU-Copyright$
 */

#include "EnergyAwareStochasticForwardingPolicyMock.h"

using namespace ARA;

EnergyAwareStochasticForwardingPolicyMock::EnergyAwareStochasticForwardingPolicyMock(EnergyAwareRoutingTable* routingTable, unsigned int seed)
        : EnergyAwareStochasticForwardingPolicy(routingTable) {

   this->initializeRandomNumberGenerator(seed);
}

