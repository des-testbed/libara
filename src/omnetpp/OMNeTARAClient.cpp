//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "OMNeTARAClient.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

// The module class needs to be registered with OMNeT++
Define_Module(OMNeTARAClient);


/**
 * 
 */
void OMNeTARAClient::initialize() {
    std::string policy = par("forwardingPolicy").stringValue();
    initializeForwardingPolicy(policy);
    deltaPhi = par("deltaPhi").doubleValue();

    for (cModule::GateIterator i(this); !i.end(); i++) {
        cGate* gate = i();
        if(gate->getType() == cGate::OUTPUT) {
            addNetworkInterface(new OMNeTGate(this, gate));
        }
    }

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

void OMNeTARAClient::handleMessage(cMessage* msg) {
    printPacket(msg);
    if(isFromUpperLayer(msg)) {
        //IPControlInfo* controlInfo = (IPControlInfo*)msg->getControlInfo();
        //controlInfo->getSrcAddr()
        //AddressPtr source = AddressPtr(new OMNeTAddress("source"));
        //AddressPtr destination = AddressPtr(new OMNeTAddress("destination"));
        //OMNeTPacket initialPacket = OMNeTPacket(source, destination, source, PacketType::DATA, getNextSequenceNumber(), "Hello ARA World");
    }
    else {
        EV << "Message from lower layer";
    }

    //OMNeTPacket* omnetPacket = (OMNeTPacket*) msg;
    //receivePacket(omnetPacket, getNetworkInterface(msg->getArrivalGate()->getIndex()));
    delete msg;
}

bool OMNeTARAClient::isFromUpperLayer(cMessage* msg) {
    std::string nameOfUpperLayergate = "upperLayerGate$i";
    std::string gateName = std::string(msg->getArrivalGate()->getName());
    return gateName.length() <= nameOfUpperLayergate.length()
        && std::equal(gateName.begin(), gateName.end(), nameOfUpperLayergate.begin());
}

void OMNeTARAClient::printPacket(cMessage* msg) {
    EV << "Message: " << msg;
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

    if(policy.compare("best") == 0) {
        this->forwardingPolicy = new BestPheromoneForwardingPolicy(&routingTable);
        EV << " set policy to BestPheromoneForwardingPolicy\n";
    }
    else if(policy.compare("stochastic") == 0) {
        this->forwardingPolicy = new OMNeTStochasticForwardingPolicy(&routingTable);
        EV << " set policy to StochasticPheromoneForwardingPolicy\n";
    }
    else {
        this->forwardingPolicy = nullptr;
        throw cRuntimeError("Unknown forwarding policy %s; Parameter forwardingPolicy must be 'best' or 'stochastic'", policy.c_str());
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
