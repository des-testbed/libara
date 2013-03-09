/*
 * $FU-Copyright$
 */

#ifndef OMNET_ENVIRONMENT_H_
#define OMNET_ENVIRONMENT_H_

#include "Environment.h"
#include "OMNeTClock.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class Environment : public cSimpleModule {

        void initialize() {
            ARA::Environment::setClock(new OMNeTClock());
        }

    };

} /* namespace omnetpp */
} /* namespace ARA */

Define_Module(ARA::omnetpp::Environment);

#endif /* OMNET_ENVIRONMENT_H_ */
