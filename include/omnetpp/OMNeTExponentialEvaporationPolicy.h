/*
 * $FU-Copyright$
 */

#ifndef OMNET_EXPONENTIAL_EVAPORATION_POLICY_H_
#define OMNET_EXPONENTIAL_EVAPORATION_POLICY_H_

#include "ExponentialEvaporationPolicy.h"
#include <omnetpp.h>

namespace ARA { 
   namespace omnetpp { 
        /**
         * This class provides the exponential evaporation function of the ant routing algorithm (ARA) 
         * for the OMNeT++ simulation framework.
         */
        class OMNeTExponentialEvaporationPolicy : public ExponentialEvaporationPolicy, public cSimpleModule {
            public:

                /**
                 * Creates a new instance of OMNeTExponentialEvaporationPolicy.
                 * Please note that the parameters need to be initialized in the
                 * initialize method due to OMNeT++s need for a standard constructor
                 */
                OMNeTExponentialEvaporationPolicy();

                virtual void initialize();
                virtual void handleMessage(cMessage *msg);
        };
    }
} /* namespace ARA */

#endif 
