/*
 * $FU-Copyright$
 */

#ifndef EARA_CONFIGURATION_H_
#define EARA_CONFIGURATION_H_

#include "ARAMacros.h"
#include "Configuration.h"
#include "EnergyAwareRoutingTable.h"
#include "EARAPacketFactory.h"

ARA_NAMESPACE_BEGIN

/**
 * This interface extends the ~Configuration interface and is used to retrieve the EARA specific configuration parameters.
 * A default implementation is available in ~BasicEARAConfiguration
 */
class EARAConfiguration : public virtual Configuration {
public:
    virtual ~EARAConfiguration() {};

    virtual EnergyAwareRoutingTable* getEnergyAwareRoutingTable() const = 0;
    virtual EARAPacketFactory* getEARAPacketFactory() const = 0;

    /**
     * Returns the maximum energy capacity which is expected in the network.
     * This will be used to calculate the percentage values from the received
     * energy.
     */
    virtual unsigned int getMaximumEnergyValue() const = 0;

    /**
     * Returns a value which indicates the impact of the Minimum energy value when the
     * route energy fitness is initialized.  The value needs to be >= 1.
     * Any value 1 <= b < 2 will favor the Minimum value over the Average.
     * Everything > 2 will put more focus on the Average value.
     * See the Master's Thesis of Friedrich Große for further explanation.
     */
    virtual float getInfluenceOfMinimumEnergyValue() const = 0;

    /**
     * Returns the time in milliseconds a client should wait for other FANTs/BANTs
     * to arrive, before it broadcasts the best ant packet of the current generation on
     * to its neighbors in the route discovery process.
     * See the Master's Thesis of Friedrich Große for further explanation.
     */
    virtual unsigned int getRouteDiscoveryDelayInMilliSeconds() const = 0;
};

ARA_NAMESPACE_END
#endif
