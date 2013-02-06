#include "OMNeTARAClient.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

/// The module class needs to be registered with OMNeT++
Define_Module(OMNeTARAClient);

/**
 * The method initializes the OMNeTARAClient class. Typically, this is
 * a task which would be provided by a constructor, but it is one of the
 * main concepts of OMNeT++ to provide such a method (and to leave 
 * constructors 'untouched'). The method parses the parameters 
 * specified in the NED file and initializes the gates.
 */
void OMNeTARAClient::initialize() {
    /// parse the policy parameter from the NED file
    std::string policy = par("policy").stringValue();
    /// check and initialize the forwarding policy
    this->initializeForwardingPolicy(policy);
    /// parse the delta phi parameter from the NED file
    this->deltaPhi = par("deltaPhi").doubleValue();
    /// parse the initial pheromone parameter from the NED file
    this->initialPhi = par("initialPhi").doubleValue();

    /// initialize the gates
    for (cModule::GateIterator i(this); !i.end(); i++) {
        cGate* gate = i();
        if(gate->getType() == cGate::OUTPUT) {
            addNetworkInterface(new OMNeTGate(this, gate));
        }
    }

    /// check if the node is a source node
    if (strcmp("source", getName()) == 0) {
        sendInitialPacket();
    }
}

void OMNeTARAClient::sendInitialPacket() {
    AddressPtr source = AddressPtr(new OMNeTAddress("source"));
    AddressPtr destination = AddressPtr(new OMNeTAddress("destination"));
    OMNeTPacket initialPacket = OMNeTPacket(source, destination, source, PacketType::DATA, getNextSequenceNumber(), "Hello ARA World");
    sendPacket(&initialPacket);
}

void OMNeTARAClient::handleMessage(cMessage *msg) {
    OMNeTPacket* omnetPacket = (OMNeTPacket*) msg;
    receivePacket(omnetPacket, getNetworkInterface(msg->getArrivalGate()->getIndex()));
    delete msg;
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

    OMNeTPacket answer = OMNeTPacket(packet->getDestination(), packet->getSource(), packet->getDestination(), PacketType::DATA, getNextSequenceNumber(), "Hello ARA World");
    sendPacket(&answer);
}

