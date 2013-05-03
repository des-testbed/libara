/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTBestPheromoneForwardingPolicy.h"

namespace ARA {
    namespace omnetpp {

        Define_Module(OMNeTBestPheromoneForwardingPolicy);

        void OMNeTBestPheromoneForwardingPolicy::initialize(){

        }

        void OMNeTBestPheromoneForwardingPolicy::handleMessage(cMessage *msg) {
            throw cRuntimeError("OMNeTBestPheromoneForwardingPolicy: handleMessage() should never be called!");
        }

    } /* namespace omnetpp */
} /* namespace ARA */
