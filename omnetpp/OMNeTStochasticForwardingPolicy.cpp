#include "OMNeTStochasticForwardingPolicy.h"

namespace ARA {
    namespace omnetpp {

        Define_Module(OMNeTStochasticForwardingPolicy);

        float OMNeTStochasticForwardingPolicy::getRandomNumber(){
           return dblrand();
        }

        void OMNeTStochasticForwardingPolicy::initialize(){

        }

        void OMNeTStochasticForwardingPolicy::handleMessage(cMessage *msg) {
            throw cRuntimeError("OMNeTStochasticForwardingPolicy: handleMessage() should never be called!");
        }

    } /* namespace omnetpp */
} /* namespace ARA */
