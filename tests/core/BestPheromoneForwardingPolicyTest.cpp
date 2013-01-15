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

#include "CppUTest/TestHarness.h"
#include "RoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "RoutingTableEntry.h"
#include "LinkedList.h"
#include "NextHop.h"
#include "PacketType.h"
#include "Exception.h" 
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

#include <iostream>

using namespace ARA;

TEST_GROUP(BestPheromoneForwardingPolicyTest) {};

TEST(BestPheromoneForwardingPolicyTest, testGetNextHop) {
    // create a routing table
    RoutingTable routingTable = RoutingTable();
    // create a destination address
    AddressMock destination = AddressMock("Destination");
    // create a network interface mock
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    // create multiple next hops
    AddressMock nextHopA = AddressMock("nextHopA");
    AddressMock nextHopB = AddressMock("nextHopB");
    AddressMock nextHopC = AddressMock("nextHopC");
    // create a packet mock 
    PacketMock packet = PacketMock();

    // set the routing table entries
    routingTable.update(&destination, &nextHopA, &interface, 1.2);
    routingTable.update(&destination, &nextHopB, &interface, 2.1);
    routingTable.update(&destination, &nextHopC, &interface, 2.3);

    // set the routing table
    BestPheromoneForwardingPolicy policy(&routingTable);
    // get next hop
    NextHop node = policy.getNextHop(&packet);

    CHECK_EQUAL(&nextHopC, node.getAddress());
}
