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
#include "LinearEvaporationPolicy.h"
#include "RoutingTableEntry.h"
#include "PacketType.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableTest) {};

TEST(RoutingTableTest, testGetPossibleNextHopsReturnsEmptyList) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr destination (new AddressMock());

    std::deque<RoutingTableEntry*>* list = routingTable.getPossibleNextHops(destination);
    CHECK(list->empty());

    delete list;
    delete evaporationPolicy;
}

TEST(RoutingTableTest, testUnregisteredAddressIsNotDeliverable) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr destinationAddress (new AddressMock());

    CHECK(routingTable.isDeliverable(destinationAddress) == false);
    delete evaporationPolicy;
}

TEST(RoutingTableTest, testPacketWithUnregisteredAddressIsNotDeliverable) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    PacketMock packet = PacketMock();

    CHECK(routingTable.isDeliverable(&packet) == false);
    delete evaporationPolicy;
}

TEST(RoutingTableTest, testUpdateRoutingTable) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    PacketMock packet = PacketMock();
    AddressPtr destination = packet.getDestination();
    AddressPtr nextHop (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    float pheromoneValue = 123.456;

    CHECK(routingTable.isDeliverable(&packet) == false);
    routingTable.update(destination, nextHop, &interface, pheromoneValue);

    CHECK(routingTable.isDeliverable(&packet));
    std::deque<RoutingTableEntry*>* nextHops = routingTable.getPossibleNextHops(&packet);
    CHECK(nextHops->size() == 1);
    RoutingTableEntry* possibleHop = nextHops->front();
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());
    delete evaporationPolicy;
}

TEST(RoutingTableTest, testOverwriteExistingEntryWithUpdate) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    PacketMock packet = PacketMock();
    AddressPtr destination = packet.getDestination();
    AddressPtr nextHop (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    float pheromoneValue = 123.456;

    // first we register a route to a destination the first time
    routingTable.update(destination, nextHop, &interface, pheromoneValue);

    CHECK(routingTable.isDeliverable(&packet));
    std::deque<RoutingTableEntry*>* nextHops = routingTable.getPossibleNextHops(&packet);
    BYTES_EQUAL(1, nextHops->size());
    RoutingTableEntry* possibleHop = nextHops->front();
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());

    // now we want to update the pheromone value of this route
    routingTable.update(destination, nextHop, &interface, 42);
    nextHops = routingTable.getPossibleNextHops(&packet);
    BYTES_EQUAL(1, nextHops->size());
    possibleHop = nextHops->front();
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(42, possibleHop->getPheromoneValue());
    delete evaporationPolicy;
}

TEST(RoutingTableTest, testGetPossibleNextHops) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination1 (new AddressMock("Destination1"));
    AddressPtr destination2 (new AddressMock("Destination2"));

    AddressPtr nextHop1a (new AddressMock("nextHop1a"));
    AddressPtr nextHop1b (new AddressMock("nextHop1b"));
    AddressPtr nextHop2 (new AddressMock("nextHop2"));
    AddressPtr nextHop3 (new AddressMock("nextHop3"));
    AddressPtr nextHop4 (new AddressMock("nextHop4"));
    NetworkInterfaceMock interface1 = NetworkInterfaceMock();
    NetworkInterfaceMock interface2 = NetworkInterfaceMock();
    NetworkInterfaceMock interface3 = NetworkInterfaceMock();

    float pheromoneValue1a = 1;
    float pheromoneValue1b = 5;
    float pheromoneValue2 = 2.3;
    float pheromoneValue3 = 4;
    float pheromoneValue4 = 2;

    routingTable.update(destination1, nextHop1a, &interface1, pheromoneValue1a);
    routingTable.update(destination1, nextHop1b, &interface1, pheromoneValue1b);
    routingTable.update(destination1, nextHop2, &interface2, pheromoneValue2);

    routingTable.update(destination2, nextHop3, &interface3, pheromoneValue3);
    routingTable.update(destination2, nextHop4, &interface1, pheromoneValue4);

    std::deque<RoutingTableEntry*>* nextHopsForDestination1 = routingTable.getPossibleNextHops(destination1);
    BYTES_EQUAL(3, nextHopsForDestination1->size());
    for (unsigned int i = 0; i < nextHopsForDestination1->size(); i++) {
        RoutingTableEntry* possibleHop = nextHopsForDestination1->at(i);
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop1a)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1a, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop1b)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1b, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop2)) {
            CHECK_EQUAL(&interface2, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue2, possibleHop->getPheromoneValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop1a, nextHop1b or nextHop2
        }
    }

    std::deque<RoutingTableEntry*>* nextHopsForDestination2 = routingTable.getPossibleNextHops(destination2);
    BYTES_EQUAL(2, nextHopsForDestination2->size());
    for (unsigned int i = 0; i < nextHopsForDestination2->size(); i++) {
        RoutingTableEntry* possibleHop = nextHopsForDestination2->at(i);
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop3)) {
            CHECK_EQUAL(&interface3, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue3, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop4)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue4, possibleHop->getPheromoneValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop3 or nextHop4
        }
    }
    delete evaporationPolicy;
}

TEST(RoutingTableTest, testGetPheromoneValue) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nextHopAddress (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // start the test

    // Should be zero because there is no known route to this destination
    LONGS_EQUAL(0, routingTable.getPheromoneValue(destination, nextHopAddress, &interface));

    routingTable.update(destination, nextHopAddress, &interface, 123);
    LONGS_EQUAL(123, routingTable.getPheromoneValue(destination, nextHopAddress, &interface));
    delete evaporationPolicy;
}

TEST(RoutingTableTest, removeEntry) {
    LinearEvaporationPolicy* evaporationPolicy = new LinearEvaporationPolicy();
    evaporationPolicy->setInterval(10000);
    // prepare the test
    RoutingTable routingTable = RoutingTable();
    routingTable.setEvaporationPolicy(evaporationPolicy);
   
    AddressPtr destination (new AddressMock("Destination"));

    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("A"));
    AddressPtr nodeC (new AddressMock("A"));

    NetworkInterfaceMock interface = NetworkInterfaceMock();

    routingTable.update(destination, nodeA, &interface, 2.5);
    routingTable.update(destination, nodeB, &interface, 2.5);
    routingTable.update(destination, nodeC, &interface, 2.5);

    // start the test
    routingTable.removeEntry(destination, nodeB, &interface);
    std::deque<RoutingTableEntry*>* possibleNextHops = routingTable.getPossibleNextHops(destination);
    for(auto& entry: *possibleNextHops) {
        if(entry->getAddress()->equals(nodeB)) {
            FAIL("The deleted hop should not longer be in the list of possible next hops");
        }
    }
    delete evaporationPolicy;
}
