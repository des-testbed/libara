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
        ForwardingPolicy* forwardingPolicy,
        float initialPheromoneValue,
        int maxNrOfRouteDiscoveryRetries,
        unsigned int routeDiscoveryTimeoutInMilliSeconds,
        unsigned int packetDeliveryDelayInMilliSeconds,
        unsigned int maximumEnergyValue,
        float influenceOfMinimumEnergyValue,
        unsigned int routeDiscoveryDelayInMilliSeconds) : BasicConfiguration(routingTable, packetFactory, evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPheromoneValue, maxNrOfRouteDiscoveryRetries, routeDiscoveryTimeoutInMilliSeconds, packetDeliveryDelayInMilliSeconds) {

    this->routingTable = routingTable;
    this->packetFactory = packetFactory;
    this->maximumEnergyValue = maximumEnergyValue;
    this->influenceOfMinimumEnergyValue = influenceOfMinimumEnergyValue;
    this->routeDiscoveryDelayInMilliSeconds = routeDiscoveryDelayInMilliSeconds;
}

EnergyAwareRoutingTable* BasicEARAConfiguration::getEnergyAwareRoutingTable() const {
    return routingTable;
}

EARAPacketFactory* BasicEARAConfiguration::getEARAPacketFactory() const {
    return packetFactory;
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

ARA_NAMESPACE_END
