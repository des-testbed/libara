#include "OMNeTLinearEvaporationPolicy.h"

namespace ARA {
    namespace omnetpp {
        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTLinearEvaporationPolicy);

        OMNeTLinearEvaporationPolicy::OMNeTLinearEvaporationPolicy():LinearEvaporationPolicy(new OMNeTTime(), new OMNeTTime()){}

        void OMNeTLinearEvaporationPolicy::initialize() {
            /// parse the policy parameter from the NED file
            this->setLinearFactor(par("q").doubleValue());
            /// parse the delta phi parameter from the NED file
            this->setThreshold(par("threshold").doubleValue());
        }

        void OMNeTLinearEvaporationPolicy::handleMessage(cMessage *msg) {
            /// this method is a stub since we don't handle messages in the evaporation policy
        }
    }
}
