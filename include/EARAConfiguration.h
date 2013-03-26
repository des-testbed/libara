/*
 * $FU-Copyright$
 */

#ifndef EARA_CONFIGURATION_H_
#define EARA_CONFIGURATION_H_

#include "Configuration.h"

namespace ARA {

    class EARAConfiguration : public Configuration {
    public:
        EARAConfiguration(EvaporationPolicy* evaporationPolicy,
                          PathReinforcementPolicy* reinforcementPolicy,
                          ForwardingPolicy* forwardingPolicy,
                          float initialPheromoneValue,
                          int maxNrOfRouteDiscoveryRetries=2,
                          unsigned int routeDiscoveryTimeoutInMilliSeconds=1000,
                          unsigned int energyDisseminationTimeoutInMilliSeconds=1000);

        /**
         * Returns the time in milliseconds that shall pass between the
         * periodic sending of the energy dissemination packets.
         */
        unsigned int getEnergyDisseminationTimeout() const;

    private:
        unsigned int energyDisseminationTimeout;
    };

} /* namespace ARA */
#endif /* CONFIGURATION_H_ */
