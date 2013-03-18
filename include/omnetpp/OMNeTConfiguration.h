/*
 * $FU-Copyright$
 */

#ifndef OMNET_CONFIGURATION_H_
#define OMNET_CONFIGURATION_H_

#include "omnetpp/OMNeTARA.h"
#include "Configuration.h"
#include "RoutingTable.h"
#include "Logger.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class OMNeTARA;

    class OMNeTConfiguration : public Configuration {
        public:
            static Logger* getLogger(cModule* module);
            static Configuration parseFrom(cModule* module);
            static RoutingTable* getRoutingTableFrom(cModule* module);
            static void initializeNetworkInterfacesOf(OMNeTARA* client);

        private:
            static cModule* getSubModule(cModule* module, const char* moduleIdentifier, const char* errorMessage);
            static cModule* getHostModule(cModule* module);
    };
}
}
#endif
