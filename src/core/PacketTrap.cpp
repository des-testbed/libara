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

#include "PacketTrap.h"

namespace ARA {

PacketTrap::PacketTrap(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

void PacketTrap::trapPacket(const Packet* packet) {
    // FIXME we need to store more than one packet for a specific destination
    Address* destination = packet->getDestination();
    trappedPackets[destination] = packet;
}

void PacketTrap::untrapPacket(const Packet* packet) {
    // FIXME we just want to remove a specific packet and not all packets for one destination
    Address* destination = packet->getDestination();
    trappedPackets.erase(destination);
}

bool PacketTrap::contains(Packet* packet) {
    Address* packetDestination = packet->getDestination();
    return trappedPackets.find(packetDestination) != trappedPackets.end();
}

bool PacketTrap::isEmpty() {
    return trappedPackets.size() == 0;
}

LinkedList<const Packet>* PacketTrap::getDeliverablePackets() {
    LinkedList<const Packet>* deliverablePackets = new LinkedList<const Packet>();

    std::unordered_map<Address*, const Packet*>::iterator iterator;
    for (iterator=trappedPackets.begin(); iterator!=trappedPackets.end(); iterator++) {
        std::pair<Address*, const Packet*> entryPair = *iterator;
        const Packet* trappedPacket = entryPair.second;
        if(routingTable->isDeliverable(trappedPacket)) {
            deliverablePackets->add(trappedPacket);
        }
    }

    return deliverablePackets;
}

} /* namespace ARA */
