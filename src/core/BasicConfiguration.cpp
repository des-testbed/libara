/*
 * $FU-Copyright$
 */

#include "BasicConfiguration.h"
#include <limits.h>
ARA_NAMESPACE_BEGIN

BasicConfiguration::BasicConfiguration(RoutingTable* routingTable, PacketFactory* packetFactory, EvaporationPolicy* evaporationPolicy, PathReinforcementPolicy* reinforcementPolicy, ForwardingPolicy* forwardingPolicy, float initialPheromoneValue, int maxNrOfRouteDiscoveryRetries, unsigned int routeDiscoveryTimeoutInMilliSeconds, unsigned int packetDeliveryDelayInMilliSeconds) {
    this->routingTable = routingTable;
    this->routingTable->setEvaporationPolicy(evaporationPolicy);
    this->packetFactory = packetFactory;
    this->evaporationPolicy = evaporationPolicy;
    this->reinforcementPolicy = reinforcementPolicy;
    this->forwardingPolicy = forwardingPolicy;
    this->initialPheromoneValue = initialPheromoneValue;
    this->maxNrOfRouteDiscoveryRetries = maxNrOfRouteDiscoveryRetries;
    this->routeDiscoveryTimeoutInMilliSeconds = routeDiscoveryTimeoutInMilliSeconds;
    this->packetDeliveryDelayInMilliSeconds = packetDeliveryDelayInMilliSeconds;

    // neighbor activity check (HELLO)
    this->neighborActivityCheckIntervalInMilliSeconds = 0; // disabled by default
    this->maxNeighborInactivityTimeInMilliSeconds = UINT_MAX; // disabled by default

    // PANTS
    this->pantIntervalInMilliSeconds = 0; // disabled by default

    // previousHop feature
    this->previousHopFeatureIsActivated = true; // enabled by default
}

RoutingTable* BasicConfiguration::getRoutingTable() {
    return routingTable;
}

PacketFactory* BasicConfiguration::getPacketFactory() {
    return packetFactory;
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

unsigned int BasicConfiguration::getNeighborActivityCheckIntervalInMilliSeconds() {
    return neighborActivityCheckIntervalInMilliSeconds;
}

unsigned int BasicConfiguration::getMaxNeighborInactivityTimeInMilliSeconds() {
    return maxNeighborInactivityTimeInMilliSeconds;
}

unsigned int BasicConfiguration::getPANTIntervalInMilliSeconds() {
    return pantIntervalInMilliSeconds;
}

void BasicConfiguration::setNeighborActivityCheckInterval(unsigned int newIntervalInMilliSeconds) {
    neighborActivityCheckIntervalInMilliSeconds = newIntervalInMilliSeconds;
}

void BasicConfiguration::setMaxNeighborInactivityTime(unsigned int newTimeInMilliSeconds) {
    maxNeighborInactivityTimeInMilliSeconds = newTimeInMilliSeconds;
}

void BasicConfiguration::setPANTInterval(unsigned int newIntervalInMilliSeconds) {
    pantIntervalInMilliSeconds = newIntervalInMilliSeconds;
}

bool BasicConfiguration::isPreviousHopFeatureActivated() {
    return previousHopFeatureIsActivated;
}

void BasicConfiguration::activatePreviousHopFeature() {
    previousHopFeatureIsActivated = true;
}

void BasicConfiguration::deactivatePreviousHopFeature() {
    previousHopFeatureIsActivated = false;
}

ARA_NAMESPACE_END
