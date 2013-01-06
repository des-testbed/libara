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
#include "LinkedList.h"
#include "RoutingTableEntry.h"
#include <sstream>

namespace ARA {

ARAClientMock::~ARAClientMock() {
    // delete the NetworkInterfaceMocks that have been created via createNewNetworkInterfaceMock
    while(interfaceMocks.isEmpty() == false) {
        NetworkInterfaceMock* mock = interfaceMocks.remove();
        delete mock;
    }
}

NextHop* ARAClientMock::getNextHop(const Packet* packet) {
    if(routingTable.isDeliverable(packet)) {
        LinkedList<RoutingTableEntry>* possibleHops = routingTable.getPossibleNextHops(packet);
        // search for the best value
        // TODO this can be replaced as soon as Michael is ready with the corresponding class
        RoutingTableEntry* bestHop = NULL;

        unsigned int nrOfPossibleRoutes = possibleHops->size();
        for (unsigned int i = 0; i < nrOfPossibleRoutes; ++i) {
            RoutingTableEntry* currentHop = possibleHops->get(i);
            if(bestHop == NULL || currentHop->getPheromoneValue() > bestHop->getPheromoneValue()) {
                bestHop = currentHop;
            }
        }

        return bestHop->getNextHop();
    }
    else {
        // TODO maybe it would be better to return a NULL Object (Pattern) instead of returning a NULL pointer
        return NULL;
    }
}

void ARAClientMock::updateRoutingTable(const Packet* packet, NetworkInterface* interface) {
    routingTable.update(packet->getSource(), packet->getSender(), interface, 10);
}

void ARAClientMock::deliverToSystem(const Packet* packet) {
    deliveredPackets.add(packet);
}

NetworkInterfaceMock* ARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    std::stringstream mockName;
    mockName << "InterfaceMock" << (interfaceMocks.size()+1);
    NetworkInterfaceMock* mock = new NetworkInterfaceMock(mockName.str().c_str(), localAddressName);
    interfaceMocks.add(mock);
    addNetworkInterface(mock);
    return mock;
}

PacketTrap* ARAClientMock::getPacketTrap() {
    return packetTrap;
}

RoutingTable* ARAClientMock::getRoutingTable() {
    return &routingTable;
}

LinkedList<const Packet>* ARAClientMock::getDeliveredPackets() {
    return &deliveredPackets;
}

} /* namespace ARA */
