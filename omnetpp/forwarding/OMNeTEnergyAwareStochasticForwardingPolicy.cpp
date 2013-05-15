/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEnergyAwareStochasticForwardingPolicy.h"

namespace ARA {
    namespace omnetpp {

        Define_Module(OMNeTEnergyAwareStochasticForwardingPolicy);

        float OMNeTEnergyAwareStochasticForwardingPolicy::getRandomNumber(){
           return dblrand();
        }

        void OMNeTEnergyAwareStochasticForwardingPolicy::initialize(){
            alpha = par("alpha").doubleValue();
            beta = par("beta").doubleValue();
        }

        void OMNeTEnergyAwareStochasticForwardingPolicy::handleMessage(cMessage *msg) {
            throw cRuntimeError("OMNeTEnergyAwareStochasticForwardingPolicy: handleMessage() should never be called!");
        }

    } /* namespace omnetpp */
} /* namespace ARA */
