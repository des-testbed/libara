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

#include "ARAClientMock.h"
#include "RoutingTableEntry.h"
#include "BestPheromoneForwardingPolicy.h"
#include "testAPI/mocks/TimeFactoryMock.h"

#include <sstream>

namespace ARA {

ARAClientMock::ARAClientMock() {
    forwardingPolicy = new BestPheromoneForwardingPolicy();
    forwardingPolicy->setRoutingTable(routingTable);
    EvaporationPolicy* policy = new LinearEvaporationPolicy(new TimeFactoryMock(), 0.2, 0.3);
    setEvaporationPolicy(policy);
}

void ARAClientMock::setEvaporationPolicy(EvaporationPolicy *policy){
    evaporationPolicy = policy;
    routingTable->setEvaporationPolicy(evaporationPolicy);
};

ARAClientMock::~ARAClientMock() {
    delete forwardingPolicy;

    // delete the NetworkInterfaceMocks that have been created via createNewNetworkInterfaceMock
    while(interfaceMocks.empty() == false) {
        NetworkInterfaceMock* mock = interfaceMocks.back();
        interfaceMocks.pop_back();
        delete mock;
    }

    routingTable->setEvaporationPolicy(NULL);
    delete evaporationPolicy;
}

ForwardingPolicy* ARAClientMock::getForwardingPolicy() {
    return forwardingPolicy;
}

void ARAClientMock::updateRoutingTable(const Packet* packet, NetworkInterface* interface) {
    routingTable->update(packet->getSource(), packet->getSender(), interface, 10);
}

void ARAClientMock::deliverToSystem(const Packet* packet) {
    deliveredPackets.push_back(packet);
}

NetworkInterfaceMock* ARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    std::stringstream mockName;
    mockName << "InterfaceMock" << (interfaceMocks.size()+1);
    NetworkInterfaceMock* mock = new NetworkInterfaceMock(mockName.str().c_str(), localAddressName);
    interfaceMocks.push_back(mock);
    addNetworkInterface(mock);
    return mock;
}

PacketTrap* ARAClientMock::getPacketTrap() {
    return packetTrap;
}

RoutingTable* ARAClientMock::getRoutingTable() {
    return routingTable;
}

std::deque<const Packet*>* ARAClientMock::getDeliveredPackets() {
    return &deliveredPackets;
}

} /* namespace ARA */
