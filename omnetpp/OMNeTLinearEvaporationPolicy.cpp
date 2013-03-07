/*
 * $FU-Copyright$
 */

#include "OMNeTLinearEvaporationPolicy.h"

namespace ARA {
    namespace omnetpp {
        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTLinearEvaporationPolicy);

        OMNeTLinearEvaporationPolicy::OMNeTLinearEvaporationPolicy() : LinearEvaporationPolicy(1, 0, (unsigned int)1000) {
            // any parameters need to be initialized in OMNeTLinearEvaporationPolicy::initialize()
        }

        void OMNeTLinearEvaporationPolicy::initialize() {
            evaporationFactor = par("evaporationFactor").doubleValue();
            threshold = par("threshold").doubleValue();
        }

        void OMNeTLinearEvaporationPolicy::handleMessage(cMessage *msg) {
            throw cRuntimeError("OMNeTLinearEvaporationPolicy: handleMessage() should never be called!");
        }
    }
}
