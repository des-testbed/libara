/*
 * $FU-Copyright$
 */

#include "BasicEARAConfiguration.h"

ARA_NAMESPACE_BEGIN

BasicEARAConfiguration::BasicEARAConfiguration(
        EnergyAwareRoutingTable* routingTable,
        EARAPacketFactory* packetFactory,
        EvaporationPolicy* evaporationPolicy,
        PathReinforcementPolicy* reinforcementPolicy,
        EARAForwardingPolicy* forwardingPolicy,
        float initialPheromoneValue,
        int maxNrOfRouteDiscoveryRetries,
        unsigned int routeDiscoveryTimeoutInMilliSeconds,
        unsigned int packetDeliveryDelayInMilliSeconds,
        unsigned int maximumEnergyValue,
        float influenceOfMinimumEnergyValue,
        unsigned int routeDiscoveryDelayInMilliSeconds,
        float peantEnergyThreshold) : BasicConfiguration(routingTable, packetFactory, evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPheromoneValue, maxNrOfRouteDiscoveryRetries, routeDiscoveryTimeoutInMilliSeconds, packetDeliveryDelayInMilliSeconds) {

    this->routingTable = routingTable;
    this->packetFactory = packetFactory;
    this->forwardingPolicy = forwardingPolicy;
    this->maximumEnergyValue = maximumEnergyValue;
    this->influenceOfMinimumEnergyValue = influenceOfMinimumEnergyValue;
    this->routeDiscoveryDelayInMilliSeconds = routeDiscoveryDelayInMilliSeconds;
    this->peantEnergyThreshold = peantEnergyThreshold;
}

EnergyAwareRoutingTable* BasicEARAConfiguration::getEnergyAwareRoutingTable() const {
    return routingTable;
}

EARAPacketFactory* BasicEARAConfiguration::getEARAPacketFactory() const {
    return packetFactory;
}

EARAForwardingPolicy* BasicEARAConfiguration::getForwardingPolicy() {
    return forwardingPolicy;
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

unsigned int BasicEARAConfiguration::getRouteDiscoveryDelayInMilliSeconds() const {
    return routeDiscoveryDelayInMilliSeconds;
}

float BasicEARAConfiguration::getPEANTEnergyThreshold() const {
    return peantEnergyThreshold;
}

void BasicEARAConfiguration::setPEANTEnergyThreshold(float peantEnergyThreshold) {
    this->peantEnergyThreshold = peantEnergyThreshold;
}

ARA_NAMESPACE_END
