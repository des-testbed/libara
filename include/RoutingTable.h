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

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_

#include "Packet.h"
#include "Address.h"
#include "NetworkInterface.h"
#include "EvaporationPolicy.h"
#include "RoutingTableEntry.h"
#include "LinearEvaporationPolicy.h"

#include <deque>
#include <unordered_map>

namespace ARA {

class RoutingTable {

public:
    RoutingTable();
    RoutingTable(EvaporationPolicy*);
    ~RoutingTable();

    float getPheromoneValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    void update(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface, float pheromoneValue);
    void removeEntry(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    std::deque<RoutingTableEntry*>* getPossibleNextHops(std::shared_ptr<Address> destination);
    std::deque<RoutingTableEntry*>* getPossibleNextHops(const Packet* packet);
    bool isDeliverable(std::shared_ptr<Address> destination);
    bool isDeliverable(const Packet* packet);
    void evaporatePheromones();
    bool exists(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

private:
    std::unordered_map<std::shared_ptr<Address>, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate> table;
    EvaporationPolicy *mEvaporationPolicy;
};

} /* namespace ARA */
#endif /* ROUTINGTABLE_H_ */
