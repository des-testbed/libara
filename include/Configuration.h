/*
 * $FU-Copyright$
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "ARAMacros.h"
#include "RoutingTable.h"
#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"

ARA_NAMESPACE_BEGIN

/**
 * This interface is used to retrieve the ARA configuration parameters.
 * A default implementation is available in ~BasicConfiguration
 */
class Configuration {
public:
    virtual ~Configuration() {};

    virtual RoutingTable* getRoutingTable() = 0;
    virtual EvaporationPolicy* getEvaporationPolicy() = 0;
    virtual PathReinforcementPolicy* getReinforcementPolicy() = 0;
    virtual ForwardingPolicy* getForwardingPolicy() = 0;
    virtual float getInitialPheromoneValue() = 0;
    virtual int getMaxNrOfRouteDiscoveryRetries() = 0;
    virtual unsigned int getRouteDiscoveryTimeoutInMilliSeconds() = 0;
    virtual unsigned int getPacketDeliveryDelayInMilliSeconds() = 0;
    virtual unsigned int getNeighborActivityCheckIntervalInMilliSeconds() = 0;
    virtual unsigned int getMaxNeighborInactivityTimeInMilliSeconds() = 0;
    virtual unsigned int getPANTIntervalInMilliSeconds() = 0;
    virtual bool isPreviousHopFeatureActivated() = 0;
};

ARA_NAMESPACE_END
#endif
