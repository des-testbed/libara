/*
 * $FU-Copyright$
 */

#ifndef OMNET_MESSAGE_DISPATCHER_H_
#define OMNET_MESSAGE_DISPATCHER_H_

#include "ARA.h"
#include "InterfaceEntry.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class ARA;

    class MessageDispatcher {
        public:
            MessageDispatcher(ARA* araClient);
            void dispatch(cMessage* message);

            bool isFromUpperLayer(cMessage* message);
            bool isARPMessage(cMessage* message);
            bool isARAMessage(cMessage* message);

        private:
            void handleUpperLayerMessage(cMessage* message);
            void handleARP(cMessage* message);
            void handleARA(cMessage* message);

            InterfaceEntry* getSourceInterfaceFrom(cMessage* message);

            ARA* araClient;
    };
}
}
#endif
