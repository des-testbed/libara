/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTARA.h"
#include "omnetpp/OMNeTPacket.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        // Register the class with the OMNeT++ simulation
        Define_Module(OMNeTARA);

        OMNeTARA::OMNeTARA() {
            messageDispatcher = new MessageDispatcher(this);
        }

        OMNeTARA::~OMNeTARA() {
            delete messageDispatcher;
            delete evaporationPolicy;
        }

        int OMNeTARA::numInitStages() const {
            return 5;
        }

        /**
         * The method initializes the ARA class. Typically, this is
         * a task which would be provided by a constructor, but it is one of the
         * main concepts of OMNeT++ to provide such a method (and to leave
         * constructors 'untouched'). The method parses the parameters
         * specified in the NED file and initializes the gates.
         */
        void OMNeTARA::initialize(int stage) {
            if(stage == 4) {
                Configuration config = OMNeTConfiguration::parseFrom(this);

                setLogger(OMNeTConfiguration::getLogger(this));
                OMNeTConfiguration::initializeNetworkInterfacesOf(this);

                routingTable = OMNeTConfiguration::getRoutingTableFrom(this);
                routingTable->setEvaporationPolicy(evaporationPolicy);

                AbstractARAClient::initialize(config, routingTable);
            }
        }

        void OMNeTARA::handleMessage(cMessage* message) {
            messageDispatcher->dispatch(message);
        }

        void OMNeTARA::deliverToSystem(const Packet* packet) {
            Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
            OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
            ASSERT(omnetPacket);

            cPacket* encapsulatedData = omnetPacket->decapsulate();
            send(encapsulatedData, "upperLayerGate$o");
        }

        void OMNeTARA::handleRouteFailure(const Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface) {
            // this will presumably be called from the context of the OMNeTTimer
            // we are now back in the context of the ARACLient simple module
            Enter_Method_Silent("OMNeTARA::packetIsNotDeliverable");

            //TODO to something with this packet other then deleting it
            delete packet;
        }

        void OMNeTARA::takeAndSend(cMessage* msg, cGate* gate, double sendDelay) {
            Enter_Method_Silent("takeAndSend(msg)");
            take(msg);
            sendDelayed(msg, sendDelay, gate);
        }

        void OMNeTARA::packetNotDeliverable(const Packet* packet) {
            //TODO report to upper layer
        }

    } /* namespace omnetpp */
} /* namespace ARA */
