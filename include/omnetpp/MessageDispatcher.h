/*
 * $FU-Copyright$
 */

#ifndef OMNET_MESSAGE_DISPATCHER_H_
#define OMNET_MESSAGE_DISPATCHER_H_

#include "OMNeTARA.h"
#include "InterfaceEntry.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class MessageDispatcher {
        public:
            MessageDispatcher(OMNeTARA* araClient);
            void dispatch(cMessage* message);

        private:
            bool isFromUpperLayer(cMessage* message);
            bool isARPMessage(cMessage* message);
            void handleUpperLayerMessage(cMessage* message);
            void handleARP(cMessage* message);
            void handleARA(cMessage* message);

            InterfaceEntry* getSourceInterfaceFrom(cMessage* message);

            OMNeTARA* araClient;
    };
}
}
#endif
