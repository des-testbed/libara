/*
 * $FU-Copyright$
 */

#ifndef BASIC_EARA_CONFIGURATION_H_
#define BASIC_EARA_CONFIGURATION_H_

#include "ARAMacros.h"
#include "BasicConfiguration.h"
#include "EARAConfiguration.h"
#include "EnergyAwareRoutingTable.h"
#include "EARAPacketFactory.h"

ARA_NAMESPACE_BEGIN

/**
 * The configuration data object for EARA. Like the BasicConfiguration, this only serves
 * as simple data object which will be passed to the algorithms initialization method and will
 * be deleted afterwards.
 */
class BasicEARAConfiguration : public virtual BasicConfiguration, public EARAConfiguration {
public:
    BasicEARAConfiguration(EnergyAwareRoutingTable* routingTable,
                           EARAPacketFactory* packetFactory,
                           EvaporationPolicy* evaporationPolicy,
                           PathReinforcementPolicy* reinforcementPolicy,
                           ForwardingPolicy* forwardingPolicy,
                           float initialPheromoneValue,
                           int maxNrOfRouteDiscoveryRetries=2,
                           unsigned int routeDiscoveryTimeoutInMilliSeconds=1000,
                           unsigned int packetDeliveryDelayInMilliSeconds=5,
                           unsigned int maximumEnergyValue=255,
                           float influenceOfMinimumEnergyValue=3);

    virtual EnergyAwareRoutingTable* getEnergyAwareRoutingTable() const;
    EARAPacketFactory* getEARAPacketFactory() const;
    virtual unsigned int getMaximumEnergyValue() const;
    virtual float getInfluenceOfMinimumEnergyValue() const;

    void setMaximumEnergyValue(unsigned int newMaxEnergy);
    void setInfluenceOfMinimumEnergyValue(float b);

private:
    EnergyAwareRoutingTable* routingTable;
    EARAPacketFactory* packetFactory;
    unsigned int maximumEnergyValue;
    float influenceOfMinimumEnergyValue;
};

ARA_NAMESPACE_END
#endif
