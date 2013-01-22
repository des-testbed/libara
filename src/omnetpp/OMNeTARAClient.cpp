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
#include <omnetpp.h>
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"
#include "OMNeTGate.h"
#include "BestPheromoneForwardingPolicy.h"
#include "StochasticForwardingPolicy.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

// The module class needs to be registered with OMNeT++
Define_Module(OMNeTARAClient);

void OMNeTARAClient::initialize() {
    // TODO make this a simulation parameter
    forwardingPolicy = new BestPheromoneForwardingPolicy(&routingTable);

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
    OMNeTPacket* msg = new OMNeTPacket(source, destination, source, PacketType::DATA, getNextSequenceNumber(), "Hello ARA World");
    sendPacket(msg);
}

void OMNeTARAClient::handleMessage(cMessage *msg) {
    OMNeTPacket* omnetPacket = (OMNeTPacket*) msg;
    receivePacket(omnetPacket, getNetworkInterface(msg->getArrivalGate()->getIndex()));
    delete msg;
}

AbstractForwardingPolicy* OMNeTARAClient::getForwardingPolicy() {
    return forwardingPolicy;
}

void OMNeTARAClient::updateRoutingTable(const Packet* packet, NetworkInterface* interface) {
    AddressPtr source = packet->getSource();
    AddressPtr sender = packet->getSender();
    float currentPheromoneValue = routingTable.getPheromoneValue(source, sender, interface);

    float deltaPhi = 10; // TODO make this a NED parameter
    float hopCountMalus = 1 / (float) packet->getHopCount();
    float newPheromoneValue = currentPheromoneValue + deltaPhi * hopCountMalus;

    routingTable.update(source, sender, interface, newPheromoneValue);
}

void OMNeTARAClient::deliverToSystem(const Packet* packet) {
    EV << getName() << " delivered a packet to the system\n";
}
