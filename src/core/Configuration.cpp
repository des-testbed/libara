/*
 * $FU-Copyright$
 */

#include "Configuration.h"

namespace ARA {

Configuration::Configuration(EvaporationPolicy* evaporationPolicy, PathReinforcementPolicy* reinforcementPolicy, ForwardingPolicy* forwardingPolicy, float initialPheromoneValue, int maxNrOfRouteDiscoveryRetries, unsigned int routeDiscoveryTimeoutInMilliSeconds) {
    this->evaporationPolicy = evaporationPolicy;
    this->reinforcementPolicy = reinforcementPolicy;
    this->forwardingPolicy = forwardingPolicy;
    this->initialPheromoneValue = initialPheromoneValue;
    this->maxNrOfRouteDiscoveryRetries = maxNrOfRouteDiscoveryRetries;
    this->routeDiscoveryTimeoutInMilliSeconds = routeDiscoveryTimeoutInMilliSeconds;
}

EvaporationPolicy* Configuration::getEvaporationPolicy() {
    return evaporationPolicy;
}

PathReinforcementPolicy* Configuration::getReinforcementPolicy() {
    return reinforcementPolicy;
}

ForwardingPolicy* Configuration::getForwardingPolicy() {
    return forwardingPolicy;
}

float Configuration::getInitialPheromoneValue() {
    return initialPheromoneValue;
}

int Configuration::getMaxNrOfRouteDiscoveryRetries() {
    return maxNrOfRouteDiscoveryRetries;
}

unsigned int Configuration::getRouteDiscoveryTimeoutInMilliSeconds() {
    return routeDiscoveryTimeoutInMilliSeconds;
}

} /* namespace ARA */
