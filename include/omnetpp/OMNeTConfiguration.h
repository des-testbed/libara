/*
 * $FU-Copyright$
 */

#ifndef OMNET_CONFIGURATION_H_
#define OMNET_CONFIGURATION_H_

#include "OMNeTARAMacros.h"
#include "Configuration.h"
#include "AbstractARAClient.h"
#include "RoutingTable.h"
#include "IInterfaceTable.h"
#include "OMNeTLogger.h"

OMNETARA_NAMESPACE_BEGIN

class OMNeTConfiguration : public Configuration {
    public:
        OMNeTConfiguration(cModule* module);

        virtual EvaporationPolicy* getEvaporationPolicy();
        virtual PathReinforcementPolicy* getReinforcementPolicy();
        virtual ForwardingPolicy* getForwardingPolicy();
        virtual float getInitialPheromoneValue();
        virtual int getMaxNrOfRouteDiscoveryRetries();
        virtual int getMaxTTL();
        virtual unsigned int getRouteDiscoveryTimeoutInMilliSeconds();
        virtual unsigned int getPacketDeliveryDelayInMilliSeconds();

        virtual RoutingTable* getRoutingTable();
        Logger* getLogger();

    protected:
        cModule* getHostModule();

    protected:
        EvaporationPolicy* evaporationPolicy;
        PathReinforcementPolicy* reinforcementPolicy;
        ForwardingPolicy* forwardingPolicy;
        float initialPheromoneValue;
        int maxNrOfRouteDiscoveryRetries;
        int maxTTL;
        unsigned int routeDiscoveryTimeoutInMilliSeconds;
        unsigned int packetDeliveryDelayInMilliSeconds;

        cModule* simpleModule;
        OMNeTLogger* logger;
};

OMNETARA_NAMESPACE_END
#endif
