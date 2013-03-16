/*
 * $FU-Copyright$
 */

#ifndef OMNET_LINEAR_PATH_REINFORCEMENT_POLICY_H_
#define OMNET_LINEAR_PATH_REINFORCEMENT_POLICY_H_

#include <omnetpp.h>
#include <csimplemodule.h>

#include "LinearPathReinforcementPolicy.h"

namespace ARA {
    namespace omnetpp {

        class OMNeTLinearPathReinforcementPolicy : public LinearPathReinforcementPolicy, public cSimpleModule {
        public:
            OMNeTLinearPathReinforcementPolicy() : LinearPathReinforcementPolicy(0) {};

        protected:
            virtual void initialize();
            virtual void handleMessage(cMessage *msg);
        };
    } 
} 
#endif 
