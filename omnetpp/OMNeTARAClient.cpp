#include "OMNeTARAClient.h"
#include "IPControlInfo.h"
#include "IPAddress.h"

namespace ARA {
namespace omnetpp {

typedef std::shared_ptr<Address> AddressPtr;

/// The module class needs to be registered with OMNeT++
Define_Module(OMNeTARAClient);

int OMNeTARAClient::numInitStages() const {
    return 5;
}

/**
 * The method initializes the OMNeTARAClient class. Typically, this is
 * a task which would be provided by a constructor, but it is one of the
 * main concepts of OMNeT++ to provide such a method (and to leave 
 * constructors 'untouched'). The method parses the parameters 
 * specified in the NED file and initializes the gates.
 */
void OMNeTARAClient::initialize(int stage) {
    if(stage == 4) {
        std::string policy = par("forwardingPolicy").stringValue();
        initializeForwardingPolicy(policy);
        deltaPhi = par("deltaPhi").doubleValue();
        initialPhi = par("initialPhi").doubleValue();

        /*for (cModule::GateIterator i(this); !i.end(); i++) {
            cGate* gate = i();
            if(gate->getType() == cGate::OUTPUT && gate->isVector()) {
                // only the vector gates go to the NICs
                addNetworkInterface(new OMNeTGate(this, gate));
            }
        }*/
        addNetworkInterface(new OMNeTGate(this, gate("arpOut")));
    }
}

void OMNeTARAClient::handleMessage(cMessage *msg) {
    if(isFromUpperLayer(msg)) {
        handleUpperLayerMessage(msg);
    }
    else {
        EV << "Message from lower layer";
        //receivePacket(omnetPacket, getNetworkInterface(msg->getArrivalGate()->getIndex()));
    }

    delete msg;
}

bool OMNeTARAClient::isFromUpperLayer(cMessage* msg) {
    std::string nameOfUpperLayergate = "upperLayerGate$i";
    std::string gateName = std::string(msg->getArrivalGate()->getName());
    return gateName.length() <= nameOfUpperLayergate.length()
        && std::equal(gateName.begin(), gateName.end(), nameOfUpperLayergate.begin());
}

void OMNeTARAClient::handleUpperLayerMessage(cMessage* msg) {
    IPControlInfo* controlInfo = (IPControlInfo*)msg->getControlInfo();
    IPAddress sourceIP = controlInfo->getSrcAddr();
    IPAddress destinationIP = controlInfo->getDestAddr();
    EV << "Handling upper layer message from " << sourceIP << " to " << destinationIP << ": "<< msg << "\n";

    AddressPtr source = AddressPtr(new OMNeTAddress(sourceIP));
    AddressPtr destination = AddressPtr(new OMNeTAddress(destinationIP));
    AddressPtr sender = source; // FIXME is this ok?
    OMNeTPacket omnetPacket = OMNeTPacket(source, destination, sender, PacketType::DATA, getNextSequenceNumber());

    sendPacket(&omnetPacket);
}

ForwardingPolicy* OMNeTARAClient::getForwardingPolicy() {
    return forwardingPolicy;
}

/**
 * The method initializes the forwarding policy of the ant routing algorithm. It checks 
 * if the policy name is a valid policy name and initializes the corresponding policy. 
 * If no policy can be found by a given name, the function sets the policy to nullptr 
 * and throws an exception. If a developer defines a new forwarding policy, the method
 * needs to be extended by means of an additional name check and object creation.
 *
 * @param policy in The name of the policy which should be checked.
 */
void OMNeTARAClient::initializeForwardingPolicy(std::string policy){
    /// we lower case each character, thus accepting strings written in camel case, only first letter upper, etc.
    std::transform(policy.begin(), policy.end(), policy.begin(), ::tolower);

    /// check if its the best pheromone forwarding policy 
    if(policy.compare("bestpheromoneforwardingpolicy") == 0){
        this->forwardingPolicy = new BestPheromoneForwardingPolicy(&routingTable);
        EV << " set policy to BestPheromoneForwardingPolicy\n";
    /// check if it is the stochastic forwarding policy
    }else if((policy.compare("stochasticforwardingpolicy") == 0) || (policy.compare("omnetstochasticforwardingpolicy") == 0)){
        /**
         * The stochastic forwarding policy is never be instantiated since it does not use
         * pseudo random number generators provided by the OMNeT++ simulation framework.
         */
        this->forwardingPolicy = new OMNeTStochasticForwardingPolicy(&routingTable);
        EV << " set policy to StochasticPheromoneForwardingPolicy\n";
    }else{
        this->forwardingPolicy = nullptr;
        throw cRuntimeError("unknown forwarding policy %s; forwarding policy must be BestPheromoneForwardingPolicy or OMNeTStochasticForwardingPolicy", policy.c_str());
    }
}

void OMNeTARAClient::updateRoutingTable(const Packet* packet, NetworkInterface* interface) {
    AddressPtr source = packet->getSource();
    AddressPtr sender = packet->getSender();
    float currentPheromoneValue = routingTable.getPheromoneValue(source, sender, interface);

    float hopCountMalus = 1 / (float) packet->getHopCount();
    float newPheromoneValue = currentPheromoneValue + deltaPhi * hopCountMalus;

    routingTable.update(source, sender, interface, newPheromoneValue);
}

void OMNeTARAClient::deliverToSystem(const Packet* packet) {
    EV << getName() << " delivered a packet to the system\n";

    OMNeTPacket answer = OMNeTPacket(packet->getDestination(), packet->getSource(), packet->getDestination(), PacketType::DATA, getNextSequenceNumber());
    sendPacket(&answer);
}

} /* namespace omnetpp */
} /* namespace ARA */
