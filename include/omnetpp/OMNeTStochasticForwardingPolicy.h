/*
 * $FU-Copyright$
 */

#ifndef OMNET_STOCHASTIC_FORWARDING_POLICY_H_
#define OMNET_STOCHASTIC_FORWARDING_POLICY_H_

#include "StochasticForwardingPolicy.h"

#include <omnetpp.h>

namespace ARA {
    namespace omnetpp {
           /**
            * The class provides a stochastic forwarding policy for class ARA. The
            * class overwrites the getRandomNumber() method of the base class. It uses a
            * pseudo-random number generator provided by OMNeT++.
            */
           class OMNeTStochasticForwardingPolicy : public cSimpleModule, public StochasticForwardingPolicy {
              protected:
                 /// The method returns a random number which uses OMNeT++ pseudo random number generators
                 float getRandomNumber();

                 virtual void initialize();

                 virtual void handleMessage(cMessage *msg);
           };

    } /* namespace omnetpp */
} /* namespace ARA */

#endif
