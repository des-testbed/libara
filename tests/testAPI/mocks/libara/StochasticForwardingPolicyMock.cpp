/*
 * $FU-Copyright$
 */

#include "StochasticForwardingPolicyMock.h"
#include <ctime>

ARA_NAMESPACE_BEGIN

StochasticForwardingPolicyMock::StochasticForwardingPolicyMock(RoutingTable* routingTable) : StochasticForwardingPolicy(routingTable) {
    this->initializeRandomNumberGenerator(time(0));
}

StochasticForwardingPolicyMock::StochasticForwardingPolicyMock(RoutingTable* routingTable, unsigned int seed) : StochasticForwardingPolicy(routingTable) {
    this->initializeRandomNumberGenerator(seed);
}

ARA_NAMESPACE_END
