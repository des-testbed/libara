/*
 * $FU-Copyright$
 */

#ifndef OMNET_LINEAR_EVAPORATION_POLICY_H_
#define OMNET_LINEAR_EVAPORATION_POLICY_H_

#include "CubicEvaporationPolicy.h"
#include <omnetpp.h>

namespace ARA { 
   namespace omnetpp {
        /**
         * This class provides the cubic evaporation function of the ant routing algorithm (ARA) 
         * for the OMNeT++ simulation framework.
         */
        class OMNeTCubicEvaporationPolicy : public CubicEvaporationPolicy, public cSimpleModule {
            public:

                /**
                 * Creates a new instance of OMNeTCubicEvaporationPolicy.
                 * Please note that the default parameter are chosen arbitrary
                 * to supply a standard constructor for OMNeT++ Define_Module()
                 *
                 * The actual parameters are loaded in the initialize() method!
                 */
                OMNeTCubicEvaporationPolicy(int plateau=0.0, float slow=0.0, float reduction=0.0, float threshold=0.0);

                virtual void initialize();
                virtual void handleMessage(cMessage *msg);
        };
    }
} /* namespace ARA */

#endif 
