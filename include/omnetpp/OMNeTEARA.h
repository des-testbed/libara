/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_CLIENT_H_
#define OMNET_EARA_CLIENT_H_

#include <omnetpp.h>
#include <algorithm>
#include <cmessage.h>
#include <unordered_map>

#include "IInterfaceTable.h"
#include "InterfaceEntry.h"

#include "Packet.h"
#include "NextHop.h"
#include "OMNeTGate.h"
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"
#include "ForwardingPolicy.h"
#include "NetworkInterface.h"
#include "AbstractARAClient.h"
#include "OMNeTRoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "OMNeTStochasticForwardingPolicy.h"

namespace ARA {
    namespace omnetpp {
        /**
         * The class provides the implementation of the energy aware ant routing algorithm (EARA)
         * for the OMNeT++ simulation framework.
         */
        class OMNeTEARA : public cSimpleModule { //, public AbstractEARAClient {
            public:
                OMNeTEARA();
                ~OMNeTEARA();

            protected:
                int numInitStages() const;
                virtual void initialize(int stage);
                virtual void handleMessage(cMessage *msg);
        };

    } /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNET_EARA_CLIENT_H_ */
