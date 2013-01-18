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
#include "AddressMock.h"

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

NetworkInterfaceMock::NetworkInterfaceMock() {
    localAddress = shared_ptr<Address>(new AddressMock("DEFAULT"));
    this->name = "NetworkInterfaceMock";
}

NetworkInterfaceMock::NetworkInterfaceMock(const std::string interfaceName) {
    localAddress = shared_ptr<Address>(new AddressMock("DEFAULT"));
    this->name = interfaceName;
}

NetworkInterfaceMock::NetworkInterfaceMock(const std::string interfaceName, const std::string localAddressName) {
    localAddress = shared_ptr<Address>(new AddressMock(localAddressName));
    this->name = interfaceName;
}

NetworkInterfaceMock::~NetworkInterfaceMock() {
    while(sentPackets.empty() == false) {
        Pair<Packet*, AddressPtr>* removedPair = sentPackets.back();
        sentPackets.pop_back();
        delete removedPair->getLeft();  // this packet has been cloned in the send method
        delete removedPair;
    }
}

std::string NetworkInterfaceMock::getName() {
    return this->name;
}

std::deque<Pair<Packet*, AddressPtr>*>* NetworkInterfaceMock::getSentPackets() {
    return &sentPackets;
}

void NetworkInterfaceMock::send(const Packet* packet, std::shared_ptr<Address> recipient) {
    Packet* copyOfPacket = packet->clone();
    std::shared_ptr<Address> copyOfAddress = std::shared_ptr<Address>(recipient);
    Pair<Packet*, AddressPtr>* pair = new Pair<Packet*, AddressPtr>(copyOfPacket, copyOfAddress);
    sentPackets.push_back(pair);
}

void NetworkInterfaceMock::broadcast(const Packet* packet) {
    std::shared_ptr<Address> broadCastAddress (new AddressMock("BROADCAST"));
    send(packet, broadCastAddress);
}

bool NetworkInterfaceMock::hasPacketBeenBroadCasted(Packet* packet) {
    for (auto& pair: sentPackets) {
        Packet* currentPacket = pair->getLeft();
        AddressPtr recipient = pair->getRight();

        if(currentPacket->equals(packet)) {
            if(recipient->isBroadCast() == true) {
                return true;
            }
        }
    }

    return false;
}

bool NetworkInterfaceMock::hasPacketBeenSend(Packet* packet) {
    for (auto& pair: sentPackets) {
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

unsigned int NetworkInterfaceMock::getNumberOfSentPackets() {
    return sentPackets.size();
}

shared_ptr<Address> NetworkInterfaceMock::getLocalAddress() {
    return shared_ptr<Address>(localAddress);
}

} /* namespace ARA */
