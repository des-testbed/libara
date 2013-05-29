/*
 * $FU-Copyright$
 */

#include "BasicConfiguration.h"

ARA_NAMESPACE_BEGIN

BasicConfiguration::BasicConfiguration(EvaporationPolicy* evaporationPolicy, PathReinforcementPolicy* reinforcementPolicy, ForwardingPolicy* forwardingPolicy, float initialPheromoneValue, int maxNrOfRouteDiscoveryRetries, unsigned int routeDiscoveryTimeoutInMilliSeconds, unsigned int packetDeliveryDelayInMilliSeconds) {
    this->evaporationPolicy = evaporationPolicy;
    this->reinforcementPolicy = reinforcementPolicy;
    this->forwardingPolicy = forwardingPolicy;
    this->initialPheromoneValue = initialPheromoneValue;
    this->maxNrOfRouteDiscoveryRetries = maxNrOfRouteDiscoveryRetries;
    this->routeDiscoveryTimeoutInMilliSeconds = routeDiscoveryTimeoutInMilliSeconds;
    this->packetDeliveryDelayInMilliSeconds = packetDeliveryDelayInMilliSeconds;
    this->neighborActivityTimeoutInMilliSeconds = 0; // disabled by default
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

unsigned int BasicConfiguration::getRouteDiscoveryTimeoutInMilliSeconds() {
    return routeDiscoveryTimeoutInMilliSeconds;
}

unsigned int BasicConfiguration::getPacketDeliveryDelayInMilliSeconds() {
    return packetDeliveryDelayInMilliSeconds;
}

unsigned int BasicConfiguration::getNeighborActivityTimeoutInMilliSeconds() {
    return neighborActivityTimeoutInMilliSeconds;
}

void BasicConfiguration::setNeighborActivityTimeoutInMs(unsigned int newTimeout) {
    neighborActivityTimeoutInMilliSeconds = newTimeout;
}

ARA_NAMESPACE_END
