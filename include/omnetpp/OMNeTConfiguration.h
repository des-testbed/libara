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

        RoutingTable* getRoutingTable();

        Logger* getLogger();
        double getUniCastDelay();
        double getBroadCastDelay();

    private:
        cModule* getHostModule();

        cModule* module;
        RoutingTable* routingTable;
        Logger* logger;

        double broadCastDelay;
        double uniCastDelay;
};

OMNETARA_NAMESPACE_END
#endif
