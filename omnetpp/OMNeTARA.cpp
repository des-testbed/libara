/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTARA.h"
#include "IPControlInfo.h"
#include "IPAddress.h"
#include "ARPPacket_m.h"
#include "SimpleLogger.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        // Register the class with the OMNeT++ simulation
        Define_Module(OMNeTARA);

        OMNeTARA::~OMNeTARA() {
            //TODO delete running route discovery timers
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
                AbstractARAClient::initialize(config);

                setLogger(OMNeTConfiguration::getLogger(this));
                OMNeTConfiguration::initializeNetworkInterfacesOf(this);

                routingTable = OMNeTConfiguration::getRoutingTableFrom(this);
                routingTable->setEvaporationPolicy(evaporationPolicy);
            }
        }

        void OMNeTARA::handleMessage(cMessage* msg) {
            if(isFromUpperLayer(msg)) {
                handleUpperLayerMessage(msg);
            }
            else if (isARPMessage(msg)) {
                handleARP(msg);
            }
            else {
                handleARA(msg);
            }
        }

        bool OMNeTARA::isFromUpperLayer(cMessage* msg) {
            std::string nameOfUpperLayergate = "upperLayerGate$i";
            std::string gateName = std::string(msg->getArrivalGate()->getName());
            return gateName.length() <= nameOfUpperLayergate.length()
                && std::equal(gateName.begin(), gateName.end(), nameOfUpperLayergate.begin());
        }

        void OMNeTARA::handleUpperLayerMessage(cMessage* msg) {
            IPControlInfo* controlInfo = (IPControlInfo*)msg->getControlInfo();
            IPAddress sourceIP = controlInfo->getSrcAddr();
            IPAddress destinationIP = controlInfo->getDestAddr();
            EV << "Handling upper layer message from " << sourceIP << " to " << destinationIP << ": "<< msg << "\n";

            AddressPtr source = AddressPtr(new OMNeTAddress(sourceIP));
            AddressPtr destination = AddressPtr(new OMNeTAddress(destinationIP));
            AddressPtr sender = source;
            OMNeTPacket* omnetPacket = new OMNeTPacket(source, destination, sender, PacketType::DATA, getNextSequenceNumber());
            omnetPacket->encapsulate(check_and_cast<cPacket*>(msg));

            sendPacket(omnetPacket);
        }

        bool OMNeTARA::isARPMessage(cMessage* msg) {
            return dynamic_cast<ARPPacket*>(msg) != NULL;
        }

        void OMNeTARA::handleARP(cMessage* msg) {
            // FIXME hasBitError() check  missing!
            delete msg->removeControlInfo();

            InterfaceEntry* arrivalInterface = getSourceInterfaceFrom(msg);
            ASSERT(arrivalInterface);

            IPRoutingDecision* routingDecision = new IPRoutingDecision();
            routingDecision->setInterfaceId(arrivalInterface->getInterfaceId());
            msg->setControlInfo(routingDecision);

            send(msg, "arpOut");
        }

        void OMNeTARA::handleARA(cMessage* msg) {
            OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(msg);
            OMNeTGate* arrivalGate = (OMNeTGate*) getNetworkInterface(msg->getArrivalGate()->getIndex());
            arrivalGate->receive(omnetPacket);
        }

        InterfaceEntry* OMNeTARA::getSourceInterfaceFrom(cMessage* msg) {
            cGate* arrivalGate = msg->getArrivalGate();
            if(arrivalGate != NULL) {
                return interfaceTable->getInterfaceByNetworkLayerGateIndex(arrivalGate->getIndex());
            }
            else {
                return NULL;
            }
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
