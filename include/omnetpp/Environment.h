/*
 * $FU-Copyright$
 */

#ifndef OMNET_ENVIRONMENT_H_
#define OMNET_ENVIRONMENT_H_

#include "../Environment.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class Environment : public cSimpleModule {
    public:
        void initialize();
        void handleMessage(cMessage *msg);

        void scheduleSelfMessage(cMessage* msg, unsigned long timeoutInMicroSeconds);

        enum MessageTypes {
            TIMER_MSG
        };
    };

} /* namespace omnetpp */
} /* namespace ARA */

#endif /* OMNET_ENVIRONMENT_H_ */
