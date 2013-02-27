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
#include "NextHop.h"
#include "PacketType.h"
#include "Exception.h" 
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/TimeFactoryMock.h"

#include <iostream>
#include <memory>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(BestPheromoneForwardingPolicyTest) {};

TEST(BestPheromoneForwardingPolicyTest, testGetNextHop) {
    float threshold = 0.2;
    float q = 0.1;
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy(new TimeFactoryMock(), threshold, q);
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr destination (new AddressMock("Destination"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // create multiple next hops
    AddressPtr nextHopA (new AddressMock("nextHopA"));
    AddressPtr nextHopB (new AddressMock("nextHopB"));
    AddressPtr nextHopC (new AddressMock("nextHopC"));

    PacketMock packet = PacketMock();

    // start the test
    routingTable.update(destination, nextHopA, &interface, 1.2);
    routingTable.update(destination, nextHopB, &interface, 2.1);
    routingTable.update(destination, nextHopC, &interface, 2.3);
    
    BestPheromoneForwardingPolicy policy =BestPheromoneForwardingPolicy();
    policy.setRoutingTable(&routingTable);
    NextHop* node = policy.getNextHop(&packet);

    // check if the chosen node matches the node with the highest pheromone value
    CHECK(nextHopC->equals(node->getAddress()));
    delete evaporationPolicy;
}
