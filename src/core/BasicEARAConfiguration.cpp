/*
 * $FU-Copyright$
 */

#include "BasicEARAConfiguration.h"

ARA_NAMESPACE_BEGIN

BasicEARAConfiguration::BasicEARAConfiguration(
        EnergyAwareRoutingTable* routingTable,
        EvaporationPolicy* evaporationPolicy,
        PathReinforcementPolicy* reinforcementPolicy,
        ForwardingPolicy* forwardingPolicy,
        float initialPheromoneValue,
        int maxNrOfRouteDiscoveryRetries,
        unsigned int routeDiscoveryTimeoutInMilliSeconds,
        unsigned int packetDeliveryDelayInMilliSeconds,
        unsigned int energyDisseminationTimeoutInMilliSeconds) : BasicConfiguration(routingTable, evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPheromoneValue, maxNrOfRouteDiscoveryRetries, routeDiscoveryTimeoutInMilliSeconds, packetDeliveryDelayInMilliSeconds) {

    this->routingTable = routingTable;
    this->energyDisseminationTimeout = energyDisseminationTimeoutInMilliSeconds;
}

EnergyAwareRoutingTable* BasicEARAConfiguration::getRoutingTable() const {
    return routingTable;
}

unsigned int BasicEARAConfiguration::getEnergyDisseminationTimeout() const {
    return energyDisseminationTimeout;
}

ARA_NAMESPACE_END
