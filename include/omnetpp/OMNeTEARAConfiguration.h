/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_CONFIGURATION_H_
#define OMNET_EARA_CONFIGURATION_H_

#include "OMNeTARAMacros.h"
#include "OMNeTConfiguration.h"
#include "EARAConfiguration.h"
#include "EnergyAwareRoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

class OMNeTEARAConfiguration : public OMNeTConfiguration, public EARAConfiguration {
    public:
        OMNeTEARAConfiguration(cModule* module);
        virtual unsigned int getEnergyDisseminationTimeout() const;

        /* It seems like C++ does not allow implementing interface methods via inheritance :( */
        virtual EvaporationPolicy* getEvaporationPolicy() {
            return OMNeTConfiguration::getEvaporationPolicy();
        }
        virtual PathReinforcementPolicy* getReinforcementPolicy() {
            return OMNeTConfiguration::getReinforcementPolicy();
        }
        virtual ForwardingPolicy* getForwardingPolicy() {
            return OMNeTConfiguration::getForwardingPolicy();
        }
        virtual float getInitialPheromoneValue() {
            return OMNeTConfiguration::getInitialPheromoneValue();
        }
        virtual int getMaxNrOfRouteDiscoveryRetries() {
            return OMNeTConfiguration::getMaxNrOfRouteDiscoveryRetries();
        }
        virtual unsigned int getRouteDiscoveryTimeoutInMilliSeconds() {
            return OMNeTConfiguration::getRouteDiscoveryTimeoutInMilliSeconds();
        }

        virtual EnergyAwareRoutingTable* getRoutingTable();

        double getMaximumBatteryLevel();
    private:
        unsigned int energyDisseminationTimeout;
        double maximumBatteryLevel;
};

OMNETARA_NAMESPACE_END
#endif
