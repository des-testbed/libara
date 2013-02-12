#include "OMNeTCubicEvaporationPolicy.h"

namespace ARA {
    namespace omnetpp {
        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTCubicEvaporationPolicy);

        OMNeTCubicEvaporationPolicy::OMNeTCubicEvaporationPolicy():CubicEvaporationPolicy(new OMNeTTime(), new OMNeTTime()){}

        void OMNeTCubicEvaporationPolicy::initialize() {
            /// parse the plateau parameter from the NED file
            this->setPlateau(par("plateau").doubleValue());
            /// parse the slow parameter from the NED file
            this->setSlow(par("slow").doubleValue());
            /// parse the reduction parameter from the NED file
            this->setReduction(par("reduction").doubleValue());
            /// parse the threshold parameter from the NED file
            this->setThreshold(par("reduction").doubleValue());
        }

        void OMNeTCubicEvaporationPolicy::handleMessage(cMessage *msg) {
            /// this method is a stub since we don't handle messages in the evaporation policy
        }
    }
}
