/*
 * $FU-Copyright$
 */

#ifndef BASIC_EARA_CONFIGURATION_H_
#define BASIC_EARA_CONFIGURATION_H_

#include "ARAMacros.h"
#include "BasicConfiguration.h"
#include "EARAConfiguration.h"
#include "EnergyAwareRoutingTable.h"

ARA_NAMESPACE_BEGIN

class BasicEARAConfiguration : public virtual BasicConfiguration, public EARAConfiguration {
public:
    BasicEARAConfiguration(EnergyAwareRoutingTable* routingTable,
                           PacketFactory* packetFactory,
                           EvaporationPolicy* evaporationPolicy,
                           PathReinforcementPolicy* reinforcementPolicy,
                           ForwardingPolicy* forwardingPolicy,
                           float initialPheromoneValue,
                           int maxNrOfRouteDiscoveryRetries=2,
                           unsigned int routeDiscoveryTimeoutInMilliSeconds=1000,
                           unsigned int packetDeliveryDelayInMilliSeconds=5,
                           unsigned int energyDisseminationTimeoutInMilliSeconds=1000);

    virtual EnergyAwareRoutingTable* getEnergyAwareRoutingTable() const;
    virtual unsigned int getEnergyDisseminationTimeout() const;

private:
    EnergyAwareRoutingTable* routingTable;
    unsigned int energyDisseminationTimeout;
};

ARA_NAMESPACE_END
#endif
