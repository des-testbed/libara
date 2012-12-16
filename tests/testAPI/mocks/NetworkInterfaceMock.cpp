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

#include "NetworkInterfaceMock.h"
#include "AddressMock.h"    // TODO this NetworkInterface implementation should not need to know AddressMock

namespace ARA {

NetworkInterfaceMock::NetworkInterfaceMock() {
    this->name = "NetworkInterfaceMock";
}

NetworkInterfaceMock::NetworkInterfaceMock(const std::string interfaceName) {
    this->name = interfaceName;
}

NetworkInterfaceMock::~NetworkInterfaceMock() {
    while(sentPackets.isEmpty() == false) {
        Pair<Packet, Address>* removedPair = sentPackets.remove();
        delete removedPair->getLeft();  // this packet has been cloned in the send method
        delete removedPair->getRight();  // the address has also been cloned in the send method
        delete removedPair;
    }
}

std::string NetworkInterfaceMock::getName() {
    return this->name;
}

LinkedList<Pair<Packet, Address>>* NetworkInterfaceMock::getSentPackets() {
    return &sentPackets;
}

void NetworkInterfaceMock::send(Packet* packet, Address* recipient) {
    Packet* copyOfPacket = packet->clone();
    Address* copyOfAddress = recipient->clone();
    Pair<Packet, Address>* pair = new Pair<Packet, Address>(copyOfPacket, copyOfAddress);
    sentPackets.add(pair);
}

void NetworkInterfaceMock::broadcast(Packet* packet) {
    Address* broadCastAddress = new AddressMock("BROADCAST");
    send(packet, broadCastAddress);
}

bool NetworkInterfaceMock::hasPacketBeenBroadCasted(Packet* packet) {
    // TODO replace this with an iterator
    unsigned int numberOfSentPackets = sentPackets.size();
    for (unsigned int i = 0; i < numberOfSentPackets; i++) {
        Pair<Packet, Address>* pair = sentPackets.get(i);
        Packet* currentPacket = pair->getLeft();
        Address* recipient = pair->getRight();

        if(currentPacket->equals(packet)) {
            if(recipient->isBroadCast() == true) {
                return true;
            }
        }
    }

    return false;
}

bool NetworkInterfaceMock::hasPacketBeenSend(Packet* packet) {
    // TODO replace this with an iterator
    unsigned int numberOfSentPackets = sentPackets.size();
    for (unsigned int i = 0; i < numberOfSentPackets; i++) {
        Pair<Packet, Address>* pair = sentPackets.get(i);
        Packet* currentPacket = pair->getLeft();

        if(currentPacket->equals(packet)) {
            return true;
        }
    }

    return false;
}

bool NetworkInterfaceMock::equals(NetworkInterface* otherInterface) {
    NetworkInterfaceMock* otherInterfaceMock = dynamic_cast<NetworkInterfaceMock*>(otherInterface);
    if(otherInterfaceMock == NULL) {
        return false;
    }
    else {
        return this->name.compare(otherInterfaceMock->name) == 0;
    }
}

} /* namespace ARA */
