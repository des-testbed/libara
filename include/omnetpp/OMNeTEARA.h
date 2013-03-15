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
        class OMNeTEARA: public cSimpleModule, public AbstractARAClient {
            public:
                OMNeTEARA() {}
                ~OMNeTEARA();

                void handleRouteFailure(const Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

            protected:
                int numInitStages() const;
                virtual void initialize(int stage);
                virtual void handleMessage(cMessage *msg);

                ForwardingPolicy* getForwardingPolicy();
                void updateRoutingTable(const Packet* packet, NetworkInterface* interface);
                void deliverToSystem(const Packet* packet);
                void packetNotDeliverable(const Packet* packet);

                void setEvaporationPolicy(EvaporationPolicy *policy);

                /**
                 * Method for friend class OMNeTGate.
                 * It switches the context to the ARAClient,
                 * takes ownership of the packet and sends it to the specified gate
                 * with the given delay.
                 */
                void takeAndSend(cMessage* msg, cGate* gate, double sendDelay = 0);

            private:

                ForwardingPolicy* forwardingPolicy;
                EvaporationPolicy* evaporationPolicy;

                IInterfaceTable* interfaceTable;

                void initializeNetworkInterfaces();
                cModule* getHostModule();
                IInterfaceTable* getInterfaceTable();
                InterfaceEntry* getSourceInterfaceFrom(cMessage* msg);
                cModule* getSubModule(const char* moduleIdentifier, const char* errorMessage);

                void initializeRoutingTable();
                void initializeForwardingPolicy();
                void initializeEvaporationPolicy();
                void initializePathReinforcementPolicy();

                bool isFromUpperLayer(cMessage* msg);
                bool isARPMessage(cMessage* msg);
                void handleUpperLayerMessage(cMessage* msg);
                void handleARP(cMessage* msg);
                void handleARA(cMessage* msg);

            friend class OMNeTGate;
        };

    } /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNET_EARA_CLIENT_H_ */
