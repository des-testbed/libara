/*
 * $FU-Copyright$
 */

#include "EARAConfiguration.h"

namespace ARA {

EARAConfiguration::EARAConfiguration(
        EvaporationPolicy* evaporationPolicy,
        PathReinforcementPolicy* reinforcementPolicy,
        ForwardingPolicy* forwardingPolicy,
        float initialPheromoneValue,
        int maxNrOfRouteDiscoveryRetries,
        unsigned int routeDiscoveryTimeoutInMilliSeconds,
        unsigned int energyDisseminationTimeoutInMilliSeconds) : Configuration(evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPheromoneValue, maxNrOfRouteDiscoveryRetries, routeDiscoveryTimeoutInMilliSeconds) {

    this->energyDisseminationTimeout = energyDisseminationTimeoutInMilliSeconds;
}

unsigned int EARAConfiguration::getEnergyDisseminationTimeout() const {
    return energyDisseminationTimeout;
}

} /* namespace ARA */
