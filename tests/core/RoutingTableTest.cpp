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

#include <UnitTest++.h>
#include "RoutingTable.h"
#include "RoutingTableEntry.h"
#include "LinkedList.h"
#include "PacketType.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

using namespace ARA;

SUITE(RoutingTableTest) {

    TEST(testGetPossibleNextHopsReturnsEmptyList) {
        RoutingTable routingTable = RoutingTable();
        AddressMock destination = AddressMock();

        LinkedList<RoutingTableEntry>* list = routingTable.getPossibleNextHops(&destination);
        CHECK(list->isEmpty());

        delete list;
    }

    TEST(testUnregisteredAddressIsNotDeliverable) {
        RoutingTable routingTable = RoutingTable();
        AddressMock destinationAddress = AddressMock();

        CHECK(routingTable.isDeliverable(&destinationAddress) == false);
    }

    TEST(testPacketWithUnregisteredAddressIsNotDeliverable) {
        RoutingTable routingTable = RoutingTable();
        PacketMock packet = PacketMock();

        CHECK(routingTable.isDeliverable(&packet) == false);
    }

    TEST(testUdateRoutingTable) {
        RoutingTable routingTable = RoutingTable();
        PacketMock packet = PacketMock();
        Address* destination = packet.getDestination();
        AddressMock nextHop = AddressMock("nextHop");
        NetworkInterfaceMock interface = NetworkInterfaceMock();
        float pheromoneValue = 123.456;

        routingTable.update(destination, &nextHop, &interface, pheromoneValue);

        CHECK(routingTable.isDeliverable(&packet));
        LinkedList<RoutingTableEntry>* nextHops = routingTable.getPossibleNextHops(&packet);
        CHECK(nextHops->size() == 1);
        RoutingTableEntry* possibleHop = nextHops->getFirst();
        CHECK_EQUAL(&nextHop, possibleHop->getAddress());
        CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
        CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());
    }

    TEST(testOverwriteExistingEntryWithUpdate) {
        RoutingTable routingTable = RoutingTable();
        PacketMock packet = PacketMock();
        Address* destination = packet.getDestination();
        AddressMock nextHop = AddressMock("nextHop");
        NetworkInterfaceMock interface = NetworkInterfaceMock();
        float pheromoneValue = 123.456;

        // first we register a route to a destination the first time
        routingTable.update(destination, &nextHop, &interface, pheromoneValue);

        CHECK(routingTable.isDeliverable(&packet));
        LinkedList<RoutingTableEntry>* nextHops = routingTable.getPossibleNextHops(&packet);
        CHECK_EQUAL(1, nextHops->size());
        RoutingTableEntry* possibleHop = nextHops->getFirst();
        CHECK_EQUAL(&nextHop, possibleHop->getAddress());
        CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
        CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());

        // now we want to update the pheromone value of this route
        routingTable.update(destination, &nextHop, &interface, 42);
        nextHops = routingTable.getPossibleNextHops(&packet);
        CHECK_EQUAL(1, nextHops->size());
        possibleHop = nextHops->getFirst();
        CHECK_EQUAL(&nextHop, possibleHop->getAddress());
        CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
        CHECK_EQUAL(42, possibleHop->getPheromoneValue());
    }

    TEST(testGetPossibleNextHops) {
        RoutingTable routingTable = RoutingTable();
        AddressMock sourceAddress = AddressMock("Source");
        AddressMock destination1 = AddressMock("Destination1");
        AddressMock destination2 = AddressMock("Destination2");

        AddressMock nextHop1a = AddressMock("nextHop1a");
        AddressMock nextHop1b = AddressMock("nextHop1b");
        AddressMock nextHop2 = AddressMock("nextHop2");
        AddressMock nextHop3 = AddressMock("nextHop3");
        AddressMock nextHop4 = AddressMock("nextHop4");
        NetworkInterfaceMock interface1 = NetworkInterfaceMock();
        NetworkInterfaceMock interface2 = NetworkInterfaceMock();
        NetworkInterfaceMock interface3 = NetworkInterfaceMock();

        float pheromoneValue1a = 1;
        float pheromoneValue1b = 5;
        float pheromoneValue2 = 2.3;
        float pheromoneValue3 = 4;
        float pheromoneValue4 = 2;

        routingTable.update(&destination1, &nextHop1a, &interface1, pheromoneValue1a);
        routingTable.update(&destination1, &nextHop1b, &interface1, pheromoneValue1b);
        routingTable.update(&destination1, &nextHop2, &interface2, pheromoneValue2);

        routingTable.update(&destination2, &nextHop3, &interface3, pheromoneValue3);
        routingTable.update(&destination2, &nextHop4, &interface1, pheromoneValue4);

        LinkedList<RoutingTableEntry>* nextHopsForDestination1 = routingTable.getPossibleNextHops(&destination1);
        CHECK_EQUAL(3, nextHopsForDestination1->size());
        for (unsigned int i = 0; i < nextHopsForDestination1->size(); i++) {
            RoutingTableEntry* possibleHop = nextHopsForDestination1->get(i);
            Address* hopAddress = possibleHop->getAddress();
            if(hopAddress == &nextHop1a) {
                CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
                CHECK_EQUAL(pheromoneValue1a, possibleHop->getPheromoneValue());
            }
            else if(hopAddress == &nextHop1b) {
                CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
                CHECK_EQUAL(pheromoneValue1b, possibleHop->getPheromoneValue());
            }
            else if(hopAddress == &nextHop2) {
                CHECK_EQUAL(&interface2, possibleHop->getNetworkInterface());
                CHECK_EQUAL(pheromoneValue2, possibleHop->getPheromoneValue());
            }
            else {
                CHECK(false); // hops for this destination must either be nextHop1a, nextHop1b or nextHop2
            }
        }

        LinkedList<RoutingTableEntry>* nextHopsForDestination2 = routingTable.getPossibleNextHops(&destination2);
        CHECK_EQUAL(2, nextHopsForDestination2->size());
        for (unsigned int i = 0; i < nextHopsForDestination2->size(); i++) {
            RoutingTableEntry* possibleHop = nextHopsForDestination2->get(i);
            Address* hopAddress = possibleHop->getAddress();
            if(hopAddress == &nextHop3) {
                CHECK_EQUAL(&interface3, possibleHop->getNetworkInterface());
                CHECK_EQUAL(pheromoneValue3, possibleHop->getPheromoneValue());
            }
            else if(hopAddress == &nextHop4) {
                CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
                CHECK_EQUAL(pheromoneValue4, possibleHop->getPheromoneValue());
            }
            else {
                CHECK(false); // hops for this destination must either be nextHop3 or nextHop4
            }
        }
    }
}
