#include "OMNeTCubicEvaporationPolicy.h"

namespace ARA {
    namespace omnetpp {
        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTCubicEvaporationPolicy);

        OMNeTCubicEvaporationPolicy::OMNeTCubicEvaporationPolicy():CubicEvaporationPolicy(new OMNeTTime(), new OMNeTTime()){}

        void OMNeTCubicEvaporationPolicy::initialize() {
            /// read the member variables from the ned file
            this->setPlateau(par("plateau").doubleValue());
            this->setSlow(par("slow").doubleValue());
            this->setReduction(par("reduction").doubleValue());
            this->setThreshold(par("reduction").doubleValue());
        }

        void OMNeTCubicEvaporationPolicy::handleMessage(cMessage *msg) {
            /// this method is a stub since we don't handle messages in the evaporation policy
        }
    }
}
