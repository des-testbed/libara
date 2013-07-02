/*
 * $FU-Copyright$
 */

#include "BasicEARAConfiguration.h"

ARA_NAMESPACE_BEGIN

BasicEARAConfiguration::BasicEARAConfiguration(
        EvaporationPolicy* evaporationPolicy,
        PathReinforcementPolicy* reinforcementPolicy,
        ForwardingPolicy* forwardingPolicy,
        float initialPheromoneValue,
        int maxNrOfRouteDiscoveryRetries,
        unsigned int routeDiscoveryTimeoutInMilliSeconds,
        unsigned int packetDeliveryDelayInMilliSeconds,
        float influenceOfMinimumEnergyValue) : BasicConfiguration(evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPheromoneValue, maxNrOfRouteDiscoveryRetries, routeDiscoveryTimeoutInMilliSeconds, packetDeliveryDelayInMilliSeconds) {

    this->influenceOfMinimumEnergyValue = influenceOfMinimumEnergyValue;
}

float BasicEARAConfiguration::getInfluenceOfMinimumEnergyValue() const {
    return influenceOfMinimumEnergyValue;
}

ARA_NAMESPACE_END
