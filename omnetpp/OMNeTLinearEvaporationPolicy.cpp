/*
 * $FU-Copyright$
 */

#include "OMNeTLinearEvaporationPolicy.h"
#include "OMNeTTimeFactory.h"

namespace ARA {
    namespace omnetpp {
        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTLinearEvaporationPolicy);

        OMNeTLinearEvaporationPolicy::OMNeTLinearEvaporationPolicy(float t, float q) : LinearEvaporationPolicy(new OMNeTTimeFactory(), t, q){

        }

        void OMNeTLinearEvaporationPolicy::initialize() {
            /// parse the policy parameter from the NED file
            this->setLinearFactor(par("q").doubleValue());
            /// parse the delta phi parameter from the NED file
            this->setThreshold(par("threshold").doubleValue());
        }

        void OMNeTLinearEvaporationPolicy::handleMessage(cMessage *msg) {
            throw cRuntimeError("OMNeTLinearEvaporationPolicy: handleMessage() should never be called!");
        }
    }
}
