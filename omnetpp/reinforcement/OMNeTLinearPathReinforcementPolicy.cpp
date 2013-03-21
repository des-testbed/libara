/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTLinearPathReinforcementPolicy.h"

namespace ARA {
    namespace omnetpp {
        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTLinearPathReinforcementPolicy);

        void OMNeTLinearPathReinforcementPolicy::initialize() {
            /// parse the delta phi parameter from the NED file
            this->setDeltaPhi(par("deltaPhi").doubleValue());
        }

        void OMNeTLinearPathReinforcementPolicy::handleMessage(cMessage *msg) {
            throw cRuntimeError("OMNeTLinearPathReinforcementPolicy: handleMessage() should never be called!");
        }
    }
}
