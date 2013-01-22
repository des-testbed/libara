/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#include "AbstractARAClient.h"
#include "PacketType.h"

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

AbstractARAClient::AbstractARAClient() {
    packetTrap = new PacketTrap(&routingTable);
}

AbstractARAClient::~AbstractARAClient() {
    delete packetTrap; //TODO we must check for and delete all packets that might still be trapped

    // delete the sequence number lists of the last received packets
    unordered_map<AddressPtr, unordered_set<unsigned int>*>::iterator iterator;
    for (iterator=lastReceivedPackets.begin(); iterator!=lastReceivedPackets.end(); iterator++) {
        pair<AddressPtr, unordered_set<unsigned int>*> entryPair = *iterator;

        // the addresses are disposed of automatically by shared_ptr
        delete entryPair.second;
    }
    lastReceivedPackets.clear();
}

void AbstractARAClient::addNetworkInterface(NetworkInterface* newInterface) {
    interfaces.push_back(newInterface);
}

NetworkInterface* AbstractARAClient::getNetworkInterface(unsigned int index) {
    return interfaces.at(index);
}

unsigned int AbstractARAClient::getNumberOfNetworkInterfaces() {
    return interfaces.size();
}

void AbstractARAClient::sendPacket(const Packet* packet) {
    if(routingTable.isDeliverable(packet)) {
        NextHop* nextHop = getNextHop(packet);
        NetworkInterface* interface = nextHop->getInterface();
        Packet* newPacket = packet->clone();
        newPacket->setSender(interface->getLocalAddress());
        newPacket->increaseHopCount();
        interface->send(newPacket, nextHop->getAddress());
        delete newPacket;
    }
    else {
        packetTrap->trapPacket(packet);
        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* fant = packet->createFANT(sequenceNr);
        broadCast(fant);
        delete fant;
    }
}

void AbstractARAClient::receivePacket(const Packet* packet, NetworkInterface* interface) {
    updateRoutingTable(packet, interface);  //FIXME Check if it is ok to update the routing table here

    if(hasBeenReceivedEarlier(packet)) {
        if(packet->isDataPacket()) {
            sendDuplicateWarning(packet, interface);
        }
        return;
    }
    else {
        registerReceivedPacket(packet);
    }

    handlePacket(packet, interface);
}

NextHop* AbstractARAClient::getNextHop(const Packet* packet) {
    ForwardingPolicy* forwardingPolicy = getForwardingPolicy();
    return forwardingPolicy->getNextHop(packet);
}

void AbstractARAClient::sendDuplicateWarning(const Packet* packet, NetworkInterface* interface) {
    Packet* duplicateWarningPacket = packet->createDuplicateWarning();
    duplicateWarningPacket->setSender(interface->getLocalAddress());
    interface->send(duplicateWarningPacket, packet->getSender());
    delete duplicateWarningPacket;
}

void AbstractARAClient::handlePacket(const Packet* packet, NetworkInterface* interface) {
    if(packet->isDataPacket()) {
        handleDataPacket(packet);
    }
    else if(packet->isAntPacket()) {
        handleAntPacket(packet);
    }
    else if(packet->getType() == PacketType::DUPLICATE_ERROR) {
        handleDuplicateErrorPacket(packet, interface);
    }
    // TODO throw exception if we can not handle this packet
}

void AbstractARAClient::handleDataPacket(const Packet* packet) {
    if(isDirectedToThisNode(packet)) {
        deliverToSystem(packet);
    }
    else {
        sendPacket(packet);
    }
}

void AbstractARAClient::handleAntPacket(const Packet* packet) {
    if(hasBeenSentByThisNode(packet) == false) {
        if(isDirectedToThisNode(packet) == false) {
            broadCast(packet);
        }
        else {
            handleAntPacketForThisNode(packet);
        }
    }
}

void AbstractARAClient::handleAntPacketForThisNode(const Packet* packet) {
    char packetType = packet->getType();

    if(packetType == PacketType::FANT) {
        Packet* bant = packet->createBANT(getNextSequenceNumber());
        broadCast(bant);
        delete bant;
    }
    else if(packetType == PacketType::BANT) {
        deque<const Packet*>* deliverablePackets = packetTrap->getDeliverablePackets();
        for(auto& deliverablePacket : *deliverablePackets) {
            sendPacket(deliverablePacket);
            packetTrap->untrapPacket(deliverablePacket); //TODO We want to remove the packet from the trap only if we got an acknowledgment back
        }
        delete deliverablePackets;
    }
    else {
        // TODO throw exception if we can not handle this packet
    }
}

void AbstractARAClient::handleDuplicateErrorPacket(const Packet* packet, NetworkInterface* interface) {
    routingTable.removeEntry(packet->getDestination(), packet->getSender(), interface);
    // TODO we can also invalidate the ack timer for the packet
}

bool AbstractARAClient::isDirectedToThisNode(const Packet* packet) const {
    AddressPtr destination = packet->getDestination();
    for(auto& interface: interfaces) {
        if(interface->getLocalAddress()->equals(destination)) {
            return true;
        }
    }
    return false;
}

bool AbstractARAClient::hasBeenSentByThisNode(const Packet* packet) const {
    AddressPtr source = packet->getSource();
    for(auto& interface: interfaces) {
        if(interface->getLocalAddress()->equals(source)) {
            return true;
        }
    }
    return false;
}

void AbstractARAClient::broadCast(const Packet* packet) {
    Packet* newPacket = packet->clone();
    newPacket->increaseHopCount();

    for(auto& interface: interfaces) {
        newPacket->setSender(interface->getLocalAddress());
        interface->broadcast(newPacket);
    }

    delete newPacket;
}

unsigned int AbstractARAClient::getNextSequenceNumber() {
    return nextSequenceNumber++;
}

bool AbstractARAClient::hasBeenReceivedEarlier(const Packet* packet) {
    AddressPtr source = packet->getSource();
    unsigned int sequenceNumber = packet->getSequenceNumber();

    unordered_map<AddressPtr, unordered_set<unsigned int>*>::const_iterator receivedPacketSeqNumbersFromSource = lastReceivedPackets.find(source);
    if(receivedPacketSeqNumbersFromSource != lastReceivedPackets.end()) {
        unordered_set<unsigned int>* sequenceNumbers = receivedPacketSeqNumbersFromSource->second;
        unordered_set<unsigned int>::const_iterator got = sequenceNumbers->find(sequenceNumber);
        if(got != sequenceNumbers->end()) {
            return true;
        }
    }
    return false;
}

void AbstractARAClient::registerReceivedPacket(const Packet* packet) {
    AddressPtr source = packet->getSource();
    unordered_map<AddressPtr, unordered_set<unsigned int>*>::const_iterator foundPacketSeqNumbersFromSource = lastReceivedPackets.find(source);

    unordered_set<unsigned int>* listOfSequenceNumbers;
    if(foundPacketSeqNumbersFromSource == lastReceivedPackets.end()) {
        // There is no record of any received packet from this source address ~> create new
        listOfSequenceNumbers = new unordered_set<unsigned int>();
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
        lastReceivedPackets[source] = listOfSequenceNumbers;
    }
    else {
        listOfSequenceNumbers = foundPacketSeqNumbersFromSource->second;
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
    }
}

} /* namespace ARA */
