/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTExponentialEvaporationPolicy.h"

namespace ARA {
    namespace omnetpp {
        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTExponentialEvaporationPolicy);

        OMNeTExponentialEvaporationPolicy::OMNeTExponentialEvaporationPolicy() : ExponentialEvaporationPolicy(1, 0, (unsigned int)1000) {
            // any parameters need to be initialized in OMNeTExponentialEvaporationPolicy::initialize()
        }

        void OMNeTExponentialEvaporationPolicy::initialize() {
            evaporationFactor = par("evaporationFactor").doubleValue();
            threshold = par("threshold").doubleValue();
        }

        void OMNeTExponentialEvaporationPolicy::handleMessage(cMessage *msg) {
            throw cRuntimeError("OMNeTExponentialEvaporationPolicy: handleMessage() should never be called!");
        }
    }
}
