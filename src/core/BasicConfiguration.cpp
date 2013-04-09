/*
 * $FU-Copyright$
 */

#include "BasicConfiguration.h"

ARA_NAMESPACE_BEGIN

BasicConfiguration::BasicConfiguration(EvaporationPolicy* evaporationPolicy, PathReinforcementPolicy* reinforcementPolicy, ForwardingPolicy* forwardingPolicy, float initialPheromoneValue, int maxNrOfRouteDiscoveryRetries, int maxTTL, unsigned int routeDiscoveryTimeoutInMilliSeconds) {
    this->evaporationPolicy = evaporationPolicy;
    this->reinforcementPolicy = reinforcementPolicy;
    this->forwardingPolicy = forwardingPolicy;
    this->initialPheromoneValue = initialPheromoneValue;
    this->maxNrOfRouteDiscoveryRetries = maxNrOfRouteDiscoveryRetries;
    this->maxTTL = maxTTL;
    this->routeDiscoveryTimeoutInMilliSeconds = routeDiscoveryTimeoutInMilliSeconds;
}

EvaporationPolicy* BasicConfiguration::getEvaporationPolicy() {
    return evaporationPolicy;
}

PathReinforcementPolicy* BasicConfiguration::getReinforcementPolicy() {
    return reinforcementPolicy;
}

ForwardingPolicy* BasicConfiguration::getForwardingPolicy() {
    return forwardingPolicy;
}

float BasicConfiguration::getInitialPheromoneValue() {
    return initialPheromoneValue;
}

int BasicConfiguration::getMaxNrOfRouteDiscoveryRetries() {
    return maxNrOfRouteDiscoveryRetries;
}

int BasicConfiguration::getMaxTTL() {
    return maxTTL;
}

unsigned int BasicConfiguration::getRouteDiscoveryTimeoutInMilliSeconds() {
    return routeDiscoveryTimeoutInMilliSeconds;
}

ARA_NAMESPACE_END
