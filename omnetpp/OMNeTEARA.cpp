/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEARA.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        Define_Module(OMNeTEARA);

        OMNeTEARA::OMNeTEARA(){
             messageDispatcher = new MessageDispatcher(this);
        }

        OMNeTEARA::~OMNeTEARA(){
            delete messageDispatcher;
        }

        void OMNeTEARA::initialize(int stage) {
            if(stage == 4) {
                Configuration config = OMNeTConfiguration::parseFrom(this);
                AbstractEARAClient::initialize(config);

                setLogger(OMNeTConfiguration::getLogger(this));
//                OMNeTConfiguration::initiali_eNetworkInterfacesOf(this);

                /**
                 * Check if the forwarding policy is of type OMNeTEnergyAwareStochasticForwardingPolicy. If
                 * otherwise - it makes no sense to continue.
                 */
                try {
                    OMNeTEnergyAwareStochasticForwardingPolicy *test = dynamic_cast<OMNeTEnergyAwareStochasticForwardingPolicy *>(forwardingPolicy);
                } catch (std::bad_alloc &exception) {
                    throw cRuntimeError("OMNeTEARA: The forwarding policy has to be of type OMNeTEnergyAwareStochasticForwardingPolicy");
                }

                routingTable = OMNeTConfiguration::getRoutingTableFrom(this);
                routingTable->setEvaporationPolicy(evaporationPolicy);
            }
        }

        void OMNeTEARA::handleMessage(cMessage *message) {
            messageDispatcher->dispatch(message);
        }

        void OMNeTEARA::handleRouteFailure(const Packet* packet, AddressPtr nextHop, NetworkInterface* interface){
            // this will presumably be called from the context of the OMNeTTimer
            // we are now back in the context of the ARACLient simple module
            Enter_Method_Silent("OMNeTEARA::packetIsNotDeliverable");

            //TODO to something with this packet other then deleting it
            delete packet;

        }

        void OMNeTEARA::deliverToSystem(const Packet* packet) {
            Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
            OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
            ASSERT(omnetPacket);

            cPacket* encapsulatedData = omnetPacket->decapsulate();
            send(encapsulatedData, "upperLayerGate$o");
        }


        void OMNeTEARA::takeAndSend(cMessage *msg, cGate* gate, double sendDelay){
            Enter_Method_Silent("takeAndSend(msg)");
            take(msg);
            sendDelayed(msg, sendDelay, gate);
        }

        void OMNeTEARA::packetNotDeliverable(const Packet* packet){
            /// TODO
        }

    }
}

