/*
 * $FU-Copyright$
 */

#ifndef OMNET_CONFIGURATION_H_
#define OMNET_CONFIGURATION_H_

#include "Configuration.h"
#include "Logger.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class OMNeTConfiguration : public Configuration {
        public:
            OMNeTConfiguration(cModule* module);

            Logger* getLogger();
            double getUniCastDelay();
            double getBroadCastDelay();

        private:
            cModule* getHostModule(cModule* module);

            Logger* logger;

            double broadCastDelay;
            double uniCastDelay;
    };
}
}
#endif
