/*
 * $FU-Copyright$
 */

#ifndef ARA_TRAFFIC_GENERATOR_H_
#define ARA_TRAFFIC_GENERATOR_H_

#include <omnetpp.h>
#include "TrafGen.h"

namespace ARA {
    namespace omnetpp {
        class TrafficGenerator : public TrafGen {
            public:
                TrafficGenerator(){};
                virtual ~TrafficGenerator(){};
                virtual void initialize(int);
                virtual void finish();

            protected:
                virtual void handleSelfMsg(cMessage*);
                virtual void handleLowerMsg(cMessage*);
                virtual void SendTraf(cMessage *msg, const char *dest);

            private:
                int lowerGateIn;
	            int lowerGateOut;
                int currentTrafficPattern;
	            double numTrafficMsgs;
        };
    }
}

#endif
