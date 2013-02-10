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

#ifndef PACKETTRAP_H_
#define PACKETTRAP_H_

#include "RoutingTable.h"
#include "Packet.h"
#include "Address.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace ARA {

class PacketTrap {
public:
    PacketTrap(RoutingTable* routingTable);
    ~PacketTrap();

    /**
     * Creates a copy of this packet and stores it until it is removed via
     * PacketTrap::untrapPacket(const Packet*).
     *
     * All copies are destroyed in the destructor or when untrapped.
     */
    void trapPacket(const Packet* packet);

    /**
     * Removes the given packet from this packet trap.
     */
    void untrapPacket(const Packet* packet);

    bool contains(Packet* packet);
    bool isEmpty();

    /**
     * Returns a new list of packets that are deliverable according to the
     * routing table associated with this packet trap.
     *
     * Note: The LinkedList must be deleted by the caller of this method
     */
    std::deque<const Packet*>* getDeliverablePackets();

private:

    bool thereIsAHashSetFor(std::shared_ptr<Address> destination);

    /**
     * This hashmap stores all trapped packets.
     * In Java we would write: HashMap<Address, HashSet<Packet>>
     * The keys are the packet destinations and the values are a
     * set of packets.
     *
     * This is a Hashmap because everytime PacketTrap::getDeliverablePackets()
     * is called we want to find all packets for a specific destination fast.
     * The Values are hashsets themselves because we also have to find individual
     * packets everytime we want to untrap a packet (i.e. after acknowledgment).
     */
    std::unordered_map<std::shared_ptr<Address>, std::unordered_set<const Packet*, PacketHash, PacketPredicate>*, AddressHash, AddressPredicate> trappedPackets;

    RoutingTable* routingTable;

};

} /* namespace ARA */
#endif /* PACKETTRAP_H_ */
