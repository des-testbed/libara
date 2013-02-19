#include "ARA.h"
#include "IPControlInfo.h"
#include "IPAddress.h"
#include "IPAddressResolver.h"
#include "ARPPacket_m.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        /// The module class needs to be registered with OMNeT++
        Define_Module(ARA);

        int ARA::numInitStages() const {
            return 5;
        }

        /**
         * The method initializes the ARA class. Typically, this is
         * a task which would be provided by a constructor, but it is one of the
         * main concepts of OMNeT++ to provide such a method (and to leave
         * constructors 'untouched'). The method parses the parameters
         * specified in the NED file and initializes the gates.
         */
        void ARA::initialize(int stage) {
            if(stage == 4) {
                deltaPhi = par("deltaPhi").doubleValue();
                initialPhi = par("initialPhi").doubleValue();

                interfaceTable = getInterfaceTable();
                initializeNetworkInterfaces();

                initializeEvaporationPolicy();
                initializeForwardingPolicy();
            }
        }

        IInterfaceTable* ARA::getInterfaceTable() {
            //TODO find a more generic way to determine the real host module
            cModule* host = getParentModule();
            cModule* parentHost = host->getParentModule();

            IInterfaceTable* interfaceTable = IPAddressResolver().findInterfaceTableOf(parentHost);
            if (interfaceTable == NULL) {
                throw cRuntimeError("Could not find the interfaceTable in host '%s'. Every %s needs to be part of a compound module that has an IInterfaceTable submodule called 'interfaceTable'", host->getFullPath().c_str(), getFullName());
            }
            return interfaceTable;
        }

        void ARA::initializeNetworkInterfaces() {
            ASSERT(interfaceTable);
            cGate* gateToARP = gate("arpOut");

            int nrOfInterfaces = interfaceTable->getNumInterfaces();
            for (int i=0; i < nrOfInterfaces; i++)         {
                InterfaceEntry* interfaceEntry = interfaceTable->getInterface(i);
                if (interfaceEntry->isLoopback() == false) {
                    addNetworkInterface(new OMNeTGate(this, gateToARP, interfaceEntry));
                }
            }
        }

        void ARA::handleMessage(cMessage* msg) {
            if(isFromUpperLayer(msg)) {
                handleUpperLayerMessage(msg);
            }
            else {
                if(isARPMessage(msg)) {
                    handleARP(msg);
                }
                else {
                    handleARA(msg);
                }
            }
        }

        bool ARA::isFromUpperLayer(cMessage* msg) {
            std::string nameOfUpperLayergate = "upperLayerGate$i";
            std::string gateName = std::string(msg->getArrivalGate()->getName());
            return gateName.length() <= nameOfUpperLayergate.length()
                && std::equal(gateName.begin(), gateName.end(), nameOfUpperLayergate.begin());
        }

        void ARA::handleUpperLayerMessage(cMessage* msg) {
            IPControlInfo* controlInfo = (IPControlInfo*)msg->getControlInfo();
            IPAddress sourceIP = controlInfo->getSrcAddr();
            IPAddress destinationIP = controlInfo->getDestAddr();
            EV << "Handling upper layer message from " << sourceIP << " to " << destinationIP << ": "<< msg << "\n";

            AddressPtr source = AddressPtr(new OMNeTAddress(sourceIP));
            AddressPtr destination = AddressPtr(new OMNeTAddress(destinationIP));
            AddressPtr sender = source; // FIXME is this ok?
            OMNeTPacket omnetPacket = OMNeTPacket(source, destination, sender, PacketType::DATA, getNextSequenceNumber());

            sendPacket(&omnetPacket);
            delete msg;
        }

        bool ARA::isARPMessage(cMessage* msg) {
            return dynamic_cast<ARPPacket*>(msg) != NULL;
        }

        void ARA::handleARP(cMessage* msg) {
            // FIXME hasBitError() check  missing!
            delete msg->removeControlInfo();

            InterfaceEntry* arrivalInterface = getSourceInterfaceFrom(msg);
            ASSERT(arrivalInterface);

            IPRoutingDecision* routingDecision = new IPRoutingDecision();
            routingDecision->setInterfaceId(arrivalInterface->getInterfaceId());
            msg->setControlInfo(routingDecision);

            send(msg, "arpOut");
        }

        void ARA::handleARA(cMessage* msg) {
            OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(msg);
            NetworkInterface* arrivalInterface = getNetworkInterface(msg->getArrivalGate()->getIndex());
            receivePacket(omnetPacket, arrivalInterface);
            delete msg;
        }

        InterfaceEntry* ARA::getSourceInterfaceFrom(cMessage* msg) {
            cGate* arrivalGate = msg->getArrivalGate();
            if(arrivalGate != NULL) {
                return interfaceTable->getInterfaceByNetworkLayerGateIndex(arrivalGate->getIndex());
            }
            else {
                return NULL;
            }
        }

        ForwardingPolicy* ARA::getForwardingPolicy() {
            return forwardingPolicy;
        }

        void ARA::initializeForwardingPolicy(){
            cModule *module = getSubmodule("forwardingPolicy");

            if(module == NULL){
                throw cRuntimeError("ARA: the forwarding policy has to be called forwardingPolicy");
            }

            this->forwardingPolicy = check_and_cast<ForwardingPolicy *>(module);
            this->forwardingPolicy->setRoutingTable(&(this->routingTable));
        }

        void ARA::initializeEvaporationPolicy(){
            cModule *module = getSubmodule("evaporationPolicy");

            if(module == NULL){
                throw cRuntimeError("ARA: the evaporation policy has to be called evaporationPolicy");
            }

            this->evaporationPolicy = check_and_cast<EvaporationPolicy *>(module);
            setEvaporationPolicy(this->evaporationPolicy);
        }


        void ARA::updateRoutingTable(const Packet* packet, NetworkInterface* interface) {
            AddressPtr source = packet->getSource();
            AddressPtr sender = packet->getSender();
            float currentPheromoneValue = routingTable.getPheromoneValue(source, sender, interface);

            float hopCountMalus = 1 / (float) packet->getHopCount();
            float newPheromoneValue = currentPheromoneValue + deltaPhi * hopCountMalus;

            routingTable.update(source, sender, interface, newPheromoneValue);
        }

        void ARA::deliverToSystem(const Packet* packet) {
            Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
            OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
            ASSERT(omnetPacket);

            cPacket* encapsulatedData = omnetPacket->decapsulate();
            send(encapsulatedData, "upperLayerGate$o");
        }

        void ARA::setEvaporationPolicy(EvaporationPolicy *policy){
            this->routingTable.setEvaporationPolicy(policy);
        }


    } /* namespace omnetpp */
} /* namespace ARA */
