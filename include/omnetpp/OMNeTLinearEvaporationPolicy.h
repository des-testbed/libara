/*
 * $FU-Copyright$
 */

#ifndef OMNET_LINEAR_EVAPORATION_POLICY_H_
#define OMNET_LINEAR_EVAPORATION_POLICY_H_

#include "LinearEvaporationPolicy.h"
#include <omnetpp.h>

namespace ARA { 
   namespace omnetpp { 
        /**
         * This class provides the linear evaporation function of the ant routing algorithm (ARA) 
         * for the OMNeT++ simulation framework.
         */
        class OMNeTLinearEvaporationPolicy : public LinearEvaporationPolicy, public cSimpleModule {
            public:

                /**
                 * Creates a new instance of OMNeTLinearEvaporationPolicy.
                 * Please note that the default parameter are chosen arbitrary
                 * to supply a standard constructor for OMNeT++ Define_Module()
                 *
                 * The actual parameters are loaded in the initialize() method!
                 */
                OMNeTLinearEvaporationPolicy(float t=0.0, float q=0.0);

                virtual void initialize();
                virtual void handleMessage(cMessage *msg);
        };
    }
} /* namespace ARA */

#endif 
