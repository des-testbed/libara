/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_CLIENT_H_
#define OMNET_EARA_CLIENT_H_

#include <omnetpp.h>
#include <cmessage.h>
#include <unordered_map>

#include "IInterfaceTable.h"
#include "InterfaceEntry.h"

#include "Packet.h"
#include "NextHop.h"
#include "ForwardingPolicy.h"
#include "NetworkInterface.h"
#include "AbstractEARAClient.h"
#include "BestPheromoneForwardingPolicy.h"

#include "OMNeTGate.h"
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"
#include "OMNeTRoutingTable.h"
#include "MessageDispatcher.h"
#include "OMNeTConfiguration.h"
#include "OMNeTStochasticForwardingPolicy.h"
#include "OMNeTEnergyAwareStochasticForwardingPolicy.h"

namespace ARA {
    namespace omnetpp {
        /**
         * The class provides the implementation of the energy aware ant routing algorithm (EARA)
         * for the OMNeT++ simulation framework.
         */
        class OMNeTEARA : public cSimpleModule, public AbstractEARAClient {
            public:
                OMNeTEARA();
                ~OMNeTEARA();

                void handleRouteFailure(const Packet* packet, AddressPtr nextHop, NetworkInterface* interface);

            protected:
                int numInitStages() const { return 5; };
                virtual void initialize(int stage);
                virtual void handleMessage(cMessage *msg);

                void deliverToSystem(const Packet* packet);
                void packetNotDeliverable(const Packet* packet);

                void takeAndSend(cMessage* msg, cGate* gate, double sendDelay = 0);

            private:
                MessageDispatcher* messageDispatcher;
                IInterfaceTable* interfaceTable;

            friend class OMNeTGate;
            friend class OMNeTConfiguration;
            friend class MessageDispatcher;
        };

    } /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNET_EARA_CLIENT_H_ */
