/*
 * $FU-Copyright$
 */

#ifndef BASIC_EARA_CONFIGURATION_H_
#define BASIC_EARA_CONFIGURATION_H_

#include "ARAMacros.h"
#include "BasicConfiguration.h"
#include "EARAConfiguration.h"

ARA_NAMESPACE_BEGIN

class BasicEARAConfiguration : public BasicConfiguration, public EARAConfiguration {
public:
    BasicEARAConfiguration(EvaporationPolicy* evaporationPolicy,
                           PathReinforcementPolicy* reinforcementPolicy,
                           ForwardingPolicy* forwardingPolicy,
                           float initialPheromoneValue,
                           int maxNrOfRouteDiscoveryRetries=2,
                           unsigned int routeDiscoveryTimeoutInMilliSeconds=1000,
                           unsigned int energyDisseminationTimeoutInMilliSeconds=1000);

    virtual unsigned int getEnergyDisseminationTimeout() const;

    /* It seems like C++ does not allow implementing interface methods via inheritance :( */
    virtual EvaporationPolicy* getEvaporationPolicy() {
        return BasicConfiguration::getEvaporationPolicy();
    }
    virtual PathReinforcementPolicy* getReinforcementPolicy() {
        return BasicConfiguration::getReinforcementPolicy();
    }
    virtual ForwardingPolicy* getForwardingPolicy() {
        return BasicConfiguration::getForwardingPolicy();
    }
    virtual float getInitialPheromoneValue() {
        return BasicConfiguration::getInitialPheromoneValue();
    }
    virtual int getMaxNrOfRouteDiscoveryRetries() {
        return BasicConfiguration::getMaxNrOfRouteDiscoveryRetries();
    }
    virtual int getMaxTTL() {
        return BasicConfiguration::getMaxTTL();
    }
    virtual unsigned int getRouteDiscoveryTimeoutInMilliSeconds() {
        return BasicConfiguration::getRouteDiscoveryTimeoutInMilliSeconds();
    }

private:
    unsigned int energyDisseminationTimeout;
};

ARA_NAMESPACE_END
#endif
