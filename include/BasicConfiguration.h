/*
 * $FU-Copyright$
 */

#ifndef BASIC_CONFIGURATION_H_
#define BASIC_CONFIGURATION_H_

#include "ARAMacros.h"
#include "Configuration.h"
#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"

namespace ARA {

/**
 * This is the default implementation of the ~Configuration interface.
 */
class BasicConfiguration : public virtual Configuration {
public:
    BasicConfiguration(EvaporationPolicy* evaporationPolicy,
                       PathReinforcementPolicy* reinforcementPolicy,
                       ForwardingPolicy* forwardingPolicy,
                       float initialPheromoneValue,
                       int maxNrOfRouteDiscoveryRetries=2,
                       unsigned int routeDiscoveryTimeoutInMilliSeconds=1000,
                       unsigned int packetDeliveryDelayInMilliSeconds=5);

    virtual EvaporationPolicy* getEvaporationPolicy();
    virtual PathReinforcementPolicy* getReinforcementPolicy();
    virtual ForwardingPolicy* getForwardingPolicy();
    virtual float getInitialPheromoneValue();
    virtual int getMaxNrOfRouteDiscoveryRetries();
    virtual unsigned int getRouteDiscoveryTimeoutInMilliSeconds();
    virtual unsigned int getPacketDeliveryDelayInMilliSeconds();
    virtual unsigned int getNeighborActivityCheckIntervalInMilliSeconds();
    virtual unsigned int getMaxNeighborInactivityTimeInMilliSeconds();
    virtual unsigned int getPANTIntervalInMilliSeconds();

    void setNeighborActivityCheckInterval(unsigned int newIntervalInMilliSeconds);
    void setMaxNeighborInactivityTime(unsigned int newTimeInMilliSeconds);
    void setPANTInterval(unsigned int newIntervalInMilliSeconds);

protected:
    EvaporationPolicy* evaporationPolicy;
    PathReinforcementPolicy* reinforcementPolicy;
    ForwardingPolicy* forwardingPolicy;
    float initialPheromoneValue;
    int maxNrOfRouteDiscoveryRetries;
    unsigned int routeDiscoveryTimeoutInMilliSeconds;
    unsigned int packetDeliveryDelayInMilliSeconds;
    unsigned int neighborActivityCheckIntervalInMilliSeconds;
    unsigned int maxNeighborInactivityTimeInMilliSeconds;
    unsigned int pantIntervalInMilliSeconds;
};

} /* namespace ARA */
#endif
