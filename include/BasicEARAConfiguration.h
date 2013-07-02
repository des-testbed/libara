/*
 * $FU-Copyright$
 */

#ifndef BASIC_EARA_CONFIGURATION_H_
#define BASIC_EARA_CONFIGURATION_H_

#include "ARAMacros.h"
#include "BasicConfiguration.h"
#include "EARAConfiguration.h"

ARA_NAMESPACE_BEGIN

/**
 * The configuration data object for EARA. Like the BasicConfiguration, this only serves
 * as simple data object which will be passed to the algorithms initialization method and will
 * be deleted afterwards.
 */
class BasicEARAConfiguration : public virtual BasicConfiguration, public EARAConfiguration {
public:

    /**
     * This creates a new object of the BasicEARAConfiguration.
     * The first parameters are the same as for ARA.
     *
     * Only the last one `influencOfMinimumEnergyValue` is EARA specific. It controls the impact
     * of the Minimum energy value when the route energy fitness is initialized.  The value needs
     * to be >= 1. Any value 1 <= b < 2 will favor the Minimum value over the Average.
     * Everything > 2 will put more focus on the Average value. See the Master's Thesis of
     * Friedrich Große for further explanation.
     */
    BasicEARAConfiguration(EvaporationPolicy* evaporationPolicy,
                           PathReinforcementPolicy* reinforcementPolicy,
                           ForwardingPolicy* forwardingPolicy,
                           float initialPheromoneValue,
                           int maxNrOfRouteDiscoveryRetries=2,
                           unsigned int routeDiscoveryTimeoutInMilliSeconds=1000,
                           unsigned int packetDeliveryDelayInMilliSeconds=5,
                           float influenceOfMinimumEnergyValue=3);

    virtual float getInfluenceOfMinimumEnergyValue() const;

private:
    float influenceOfMinimumEnergyValue;
};

ARA_NAMESPACE_END
#endif
