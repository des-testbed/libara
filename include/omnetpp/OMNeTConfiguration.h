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
#include "Logger.h"

OMNETARA_NAMESPACE_BEGIN

class OMNeTConfiguration : public Configuration {
    public:
        OMNeTConfiguration(cModule* module);
        void initializeNetworkInterfacesOf(AbstractARAClient* client);

        virtual EvaporationPolicy* getEvaporationPolicy();
        virtual PathReinforcementPolicy* getReinforcementPolicy();
        virtual ForwardingPolicy* getForwardingPolicy();
        virtual float getInitialPheromoneValue();
        virtual int getMaxNrOfRouteDiscoveryRetries();
        virtual unsigned int getRouteDiscoveryTimeoutInMilliSeconds();

        virtual RoutingTable* getRoutingTable();
        Logger* getLogger();
        double getUniCastDelay();
        double getBroadCastDelay();

    protected:
        cModule* getHostModule();

    protected:
        EvaporationPolicy* evaporationPolicy;
        PathReinforcementPolicy* reinforcementPolicy;
        ForwardingPolicy* forwardingPolicy;
        float initialPheromoneValue;
        int maxNrOfRouteDiscoveryRetries;
        unsigned int routeDiscoveryTimeoutInMilliSeconds;

        cModule* simpleModule;
        Logger* logger;

        double broadCastDelay;
        double uniCastDelay;
};

OMNETARA_NAMESPACE_END
#endif
