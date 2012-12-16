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

namespace ARA {

AbstractARAClient::AbstractARAClient() {

}

AbstractARAClient::~AbstractARAClient() {

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

void AbstractARAClient::sendPacket(Packet* packet) {
    if(routingTable.isDeliverable(packet)) {
        NextHop* nextHop = getNextHop(packet);
        NetworkInterface* interface = nextHop->getInterface();
        Packet* newPacket = packet->clone();
        newPacket->setHopCount(packet->getHopCount() + 1);
        interface->send(newPacket, nextHop->getAddress());
        delete newPacket;
    }
    else {
        packetTrap.trapPacket(packet);
        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* fant = packet->createFANT(sequenceNr);
        broadCast(fant);
    }
}

void AbstractARAClient::broadCast(Packet* packet) {
    //TODO replace this with iterator which should be faster
    unsigned int nrOfInterfaces = interfaces.size();
    for(unsigned int i=0; i<nrOfInterfaces; i++) {
        NetworkInterface* interface = interfaces.get(i);
        interface->broadcast(packet);
    }
}

unsigned int AbstractARAClient::getNextSequenceNumber() {
    return nextSequenceNumber++;
}

} /* namespace ARA */
