/*
 * $FU-Copyright$
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"

namespace ARA {

    class Configuration {
    public:
        Configuration(EvaporationPolicy* evaporationPolicy, PathReinforcementPolicy* reinforcementPolicy, ForwardingPolicy* forwardingPolicy, float initialPheromoneValue, int maxNrOfRouteDiscoveryRetries=2, unsigned int routeDiscoveryTimeoutInMilliSeconds=1000);

        EvaporationPolicy* getEvaporationPolicy();
        PathReinforcementPolicy* getReinforcementPolicy();
        ForwardingPolicy* getForwardingPolicy();
        float getInitialPheromoneValue();
        int getMaxNrOfRouteDiscoveryRetries();
        unsigned int getRouteDiscoveryTimeoutInMilliSeconds();

    private:
        EvaporationPolicy* evaporationPolicy;
        PathReinforcementPolicy* reinforcementPolicy;
        ForwardingPolicy* forwardingPolicy;
        float initialPheromoneValue;
        int maxNrOfRouteDiscoveryRetries;
        unsigned int routeDiscoveryTimeoutInMilliSeconds;
    };

} /* namespace ARA */
#endif /* CONFIGURATION_H_ */
