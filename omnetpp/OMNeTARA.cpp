/*
 * $FU-Copyright$
 */

#include "OMNeTARA.h"
#include "IPControlInfo.h"
#include "IPAddress.h"
#include "IPAddressResolver.h"
#include "ARPPacket_m.h"
#include "SimpleLogger.h"
#include "Environment.h"

namespace ARA {
    namespace omnetpp {
        typedef std::shared_ptr<Address> AddressPtr;

        /// The module class needs to be registered with OMNeT++
        Define_Module(OMNeTARA);

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
            if(stage == 0) {
                Environment::setClock(new OMNeTClock());
            }
            else if(stage == 4) {
                initialPhi = par("initialPhi").doubleValue();

                interfaceTable = getInterfaceTable();
				initializeNetworkInterfaces();
                setLogger(new SimpleLogger(getHostModule()->getName()));
				initializeRoutingTable();
                initializeEvaporationPolicy();
                initializeForwardingPolicy();
                initializePathReinforcementPolicy();
            }
        }

        cModule* OMNeTARA::getHostModule() {
            //TODO find a more generic way to determine the real host module
            cModule* parent = getParentModule();
            cModule* grandParent = parent->getParentModule();
            return grandParent;
        }

        IInterfaceTable* OMNeTARA::getInterfaceTable() {
            cModule* host = getHostModule();
            IInterfaceTable* interfaceTable = IPAddressResolver().findInterfaceTableOf(host);
            if (interfaceTable == NULL) {
                throw cRuntimeError("Could not find the interfaceTable in host '%s'. Every %s needs to be part of a compound module that has an IInterfaceTable submodule called 'interfaceTable'", host->getFullPath().c_str(), getFullName());
            }
            return interfaceTable;
        }

        void OMNeTARA::initializeNetworkInterfaces() {
            double broadCastDelay = par("broadCastDelay").doubleValue();
            double uniCastDelay = par("uniCastDelay").doubleValue();

            ASSERT(interfaceTable);
            cGate* gateToARP = gate("arpOut");

            int nrOfInterfaces = interfaceTable->getNumInterfaces();
            for (int i=0; i < nrOfInterfaces; i++)         {
                InterfaceEntry* interfaceEntry = interfaceTable->getInterface(i);
                if (interfaceEntry->isLoopback() == false) {
                    addNetworkInterface(new OMNeTGate(this, gateToARP, interfaceEntry, broadCastDelay, uniCastDelay));
                }
            }
        }

        void OMNeTARA::handleMessage(cMessage* msg) {
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
            NetworkInterface* arrivalInterface = getNetworkInterface(msg->getArrivalGate()->getIndex());
            receivePacket(omnetPacket, arrivalInterface);
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

        ForwardingPolicy* OMNeTARA::getForwardingPolicy() {
            return forwardingPolicy;
        }

        cModule* OMNeTARA::getSubModule(const char* moduleIdentifier, const char* errorMessage){
            cModule* host = getParentModule();
            cModule* module = host->getSubmodule(moduleIdentifier);
   
            if(module == NULL){
                throw cRuntimeError(errorMessage);
            }

            return module;
        }

        void OMNeTARA::initializeForwardingPolicy(){
            try{
                cModule *module = this->getSubModule("forwardingPolicy", "ARA: the forwarding policy has to be called forwardingPolicy");
                this->forwardingPolicy = check_and_cast<ForwardingPolicy *>(module);
                this->forwardingPolicy->setRoutingTable(this->routingTable);
            }catch(cRuntimeError &error){
                throw;
            }
        }

        void OMNeTARA::initializeEvaporationPolicy(){
            try{
                cModule *module = this->getSubModule("evaporationPolicy", "ARA: the evaporation policy has to be called evaporationPolicy");
                this->evaporationPolicy = check_and_cast<EvaporationPolicy *>(module);
                setEvaporationPolicy(this->evaporationPolicy);
            }catch(cRuntimeError &error){
                throw;
            }
        }

        void OMNeTARA::initializeRoutingTable(){
            try{
                cModule *module = this->getSubModule("routingTableStatistics", "ARA: the routing table has to be called routingTableStatistics");
                this->routingTable = check_and_cast<RoutingTable *>(module);
            }catch(cRuntimeError &error){
                throw;
            }
        }

        void OMNeTARA::initializePathReinforcementPolicy(){
            try{
                cModule *module = this->getSubModule("pathReinforcementPolicy", "ARA: the routing table has to be called pathReinforcementPolicy");
                this->pathReinforcementPolicy = check_and_cast<PathReinforcementPolicy *>(module);
                this->pathReinforcementPolicy->setRoutingTable(this->routingTable);
            }catch(cRuntimeError &error){
                throw;
            }
        }


        void OMNeTARA::updateRoutingTable(const Packet* packet, NetworkInterface* interface) {
            AddressPtr source = packet->getSource();
            AddressPtr sender = packet->getSender();
            float currentPheromoneValue = routingTable->getPheromoneValue(source, sender, interface);

            float hopCountMalus = 1 / (float) packet->getHopCount();
            float newPheromoneValue = currentPheromoneValue + deltaPhi * hopCountMalus;

            routingTable->update(source, sender, interface, newPheromoneValue);
        }

        void OMNeTARA::deliverToSystem(const Packet* packet) {
            Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
            OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
            ASSERT(omnetPacket);

            cPacket* encapsulatedData = omnetPacket->decapsulate();
            send(encapsulatedData, "upperLayerGate$o");
        }

        void OMNeTARA::packetIsNotDeliverable(const Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface) {
            //TODO to something with this packet other then deleting it
            delete packet;
        }

        void OMNeTARA::setEvaporationPolicy(EvaporationPolicy *policy){
            this->routingTable->setEvaporationPolicy(policy);
        }


    } /* namespace omnetpp */
} /* namespace ARA */
