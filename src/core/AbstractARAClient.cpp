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

AbstractARAClient::AbstractARAClient() {
    packetTrap = new PacketTrap(&routingTable);
}

AbstractARAClient::~AbstractARAClient() {
    delete packetTrap; //TODO we must check for and delete all packets that might still be trapped

    // delete the addresses and sequence number lists of the last received packets
    unordered_map<Address*, unordered_set<unsigned int>*>::iterator iterator;
    for (iterator=lastReceivedPackets.begin(); iterator!=lastReceivedPackets.end(); iterator++) {
        std::pair<Address* const, unordered_set<unsigned int>*> entryPair = *iterator;

        delete entryPair.first;
        delete entryPair.second;
    }
    lastReceivedPackets.clear();
}

void AbstractARAClient::addNetworkInterface(NetworkInterface* newInterface) {
    interfaces.add(newInterface);
}

NetworkInterface* AbstractARAClient::getNetworkInterface(unsigned int index) {
    return interfaces.get(index);
}

unsigned int AbstractARAClient::getNumberOfNetworkInterfaces() {
    return interfaces.size();
}

void AbstractARAClient::sendPacket(const Packet* packet) {
    if(routingTable.isDeliverable(packet)) {
        NextHop* nextHop = getNextHop(packet);
        NetworkInterface* interface = nextHop->getInterface();
        Packet* newPacket = packet->clone();
        newPacket->setSender(interface->getLocalAddress()->clone());
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
    if(hasBeenReceivedEarlier(packet)) {
        //TODO what if the route over which we have received this packet earlier is shorter?
        sendDuplicateWarning(packet->getSender(), interface);
        return;
    }

    registerReceivedPacket(packet);
    updateRoutingTable(packet, interface);
    handlePacket(packet);
}

void AbstractARAClient::sendDuplicateWarning(Address* recipient, NetworkInterface* interface) {
    Address* localAddress = interface->getLocalAddress()->clone();
    unsigned int sequenceNumber = getNextSequenceNumber();
    Packet duplicateWarningPacket = Packet(localAddress, recipient->clone(), localAddress, PacketType::DUPLICATE_WARNING, sequenceNumber);
    interface->send(&duplicateWarningPacket, recipient);
}

void AbstractARAClient::handlePacket(const Packet* packet) {
    if(packet->isDataPacket()) {
        handleDataPacket(packet);
    }
    else if(packet->isAntPacket()) {
        handleAntPacket(packet);
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
    if(isDirectedToThisNode(packet) == false) {
        broadCast(packet);
    }
    else {
        handleAntPacketForThisNode(packet);
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
        LinkedList<const Packet>* deliverablePackets = packetTrap->getDeliverablePackets();
        //TODO replace this with an iterator
        unsigned int nrOfDeliverablePackets = deliverablePackets->size();
        for (unsigned int i = 0; i < nrOfDeliverablePackets; i++) {
            const Packet* deliverablePacket = deliverablePackets->get(i);
            sendPacket(deliverablePacket);
            packetTrap->untrapPacket(deliverablePacket); //TODO We want to remove the packet from the trap only if we got an acknowledgment back
        }
        delete deliverablePackets;
    }
    else {
        // TODO throw exception if we can not handle this packet
    }
}

bool AbstractARAClient::isDirectedToThisNode(const Packet* packet) {
    Address* destination = packet->getDestination();
    //TODO replace this with an iterator
    unsigned int numberOfInterfaces = interfaces.size();
    for (unsigned int i = 0; i < numberOfInterfaces; i++) {
        NetworkInterface* interface = interfaces.get(i);
        if(interface->getLocalAddress()->equals(destination)) {
            return true;
        }
    }

    return false;
}

void AbstractARAClient::broadCast(const Packet* packet) {
    Packet* newPacket = packet->clone();
    newPacket->increaseHopCount();

    //TODO replace this with iterator which should be faster
    unsigned int nrOfInterfaces = interfaces.size();
    for(unsigned int i=0; i<nrOfInterfaces; i++) {
        NetworkInterface* interface = interfaces.get(i);
        newPacket->setSender(interface->getLocalAddress()->clone());
        interface->broadcast(newPacket);
    }

    delete newPacket;
}

unsigned int AbstractARAClient::getNextSequenceNumber() {
    return nextSequenceNumber++;
}

bool AbstractARAClient::hasBeenReceivedEarlier(const Packet* packet) {
    Address* source = packet->getSource();
    unsigned int sequenceNumber = packet->getSequenceNumber();

    unordered_map<Address*, unordered_set<unsigned int>*>::const_iterator receivedPacketSeqNumbersFromSource = lastReceivedPackets.find(source);
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
    Address* source = packet->getSource();
    unordered_map<Address*, unordered_set<unsigned int>*>::const_iterator foundPacketSeqNumbersFromSource = lastReceivedPackets.find(source);

    unordered_set<unsigned int>* listOfSequenceNumbers;
    if(foundPacketSeqNumbersFromSource == lastReceivedPackets.end()) {
        // There is no record of any received packet from this source address ~> create new
        listOfSequenceNumbers = new unordered_set<unsigned int>();
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
        lastReceivedPackets[source->clone()] = listOfSequenceNumbers;
    }
    else {
        listOfSequenceNumbers = foundPacketSeqNumbersFromSource->second;
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
    }
}

} /* namespace ARA */
