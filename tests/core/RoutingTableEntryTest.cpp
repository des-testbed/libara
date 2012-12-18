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
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

using namespace ARA;

TEST_GROUP(RoutingTableEntryTest) {};

TEST(RoutingTableEntryTest, testGetters) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressMock nextHop = AddressMock();
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(&nextHop, &interface, pheromoneValue);

    CHECK_EQUAL(&nextHop, entry.getAddress());
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, entry.getPheromoneValue());
}

TEST(RoutingTableEntryTest, testSetPheromoneValue) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressMock address = AddressMock();
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(&address, &interface, pheromoneValue);

    entry.setPheromoneValue(42);
    CHECK_EQUAL(&address, entry.getAddress());
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(42, entry.getPheromoneValue());
}

TEST(RoutingTableEntryTest, testGetNextHop) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressMock address = AddressMock();
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(&address, &interface, pheromoneValue);

    NextHop* nextHop = entry.getNextHop();
    CHECK(nextHop->getAddress()->equals(&address));
    CHECK(nextHop->getInterface()->equals(&interface));
}
