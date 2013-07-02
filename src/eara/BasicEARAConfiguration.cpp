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
        unsigned int maximumEnergyValue,
        float influenceOfMinimumEnergyValue) : BasicConfiguration(evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPheromoneValue, maxNrOfRouteDiscoveryRetries, routeDiscoveryTimeoutInMilliSeconds, packetDeliveryDelayInMilliSeconds) {

    this->maximumEnergyValue = maximumEnergyValue;
    this->influenceOfMinimumEnergyValue = influenceOfMinimumEnergyValue;
}

unsigned int BasicEARAConfiguration::getMaximumEnergyValue() const {
    return maximumEnergyValue;
}

float BasicEARAConfiguration::getInfluenceOfMinimumEnergyValue() const {
    return influenceOfMinimumEnergyValue;
}

void BasicEARAConfiguration::setMaximumEnergyValue(unsigned int newMaxEnergy) {
    maximumEnergyValue = newMaxEnergy;
}

void BasicEARAConfiguration::setInfluenceOfMinimumEnergyValue(float b) {
    influenceOfMinimumEnergyValue = b;
}

ARA_NAMESPACE_END
