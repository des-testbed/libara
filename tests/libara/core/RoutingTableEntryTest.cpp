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

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableEntryTest) {};

TEST(RoutingTableEntryTest, testGetters) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr nextHop (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(nextHop, &interface, pheromoneValue);

    CHECK(nextHop->equals(entry.getAddress()));
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, entry.getPheromoneValue());
}

/// the method tests the overloaded output stream operator
TEST(RoutingTableEntryTest, testOutputStreamOperator) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    std::shared_ptr<AddressMock> address;
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, &interface, pheromoneValue);
/// TODO: find a better way to test the ostream
    std::cout << entry << std::endl;
}

TEST(RoutingTableEntryTest, testSetPheromoneValue) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, &interface, pheromoneValue);

    entry.setPheromoneValue(42);
    CHECK(address->equals(entry.getAddress()));
    CHECK_EQUAL(&interface, entry.getNetworkInterface());
    CHECK_EQUAL(42, entry.getPheromoneValue());
}

TEST(RoutingTableEntryTest, testGetNextHop) {
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr address (new AddressMock());
    float pheromoneValue = 1.234;
    RoutingTableEntry entry = RoutingTableEntry(address, &interface, pheromoneValue);

    NextHop* nextHop = entry.getNextHop();
    CHECK(nextHop->getAddress()->equals(address));
    CHECK(nextHop->getInterface()->equals(&interface));
}
