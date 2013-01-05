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
#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "NetworkInterface.h"
#include "NextHop.h"

using namespace ARA;

TEST_GROUP(ARAClientMockTest) {};

TEST(ARAClientMockTest, testGetPacketTrap){
    ARAClientMock client = ARAClientMock();
    PacketTrap* packetTrap = client.getPacketTrap();
    CHECK(packetTrap != NULL);
}

TEST(ARAClientMockTest, testGetRoutingTable){
    ARAClientMock client = ARAClientMock();
    RoutingTable* routingTable = client.getRoutingTable();
    CHECK(routingTable != NULL);
}

TEST(ARAClientMockTest, testGetNetworkInterfaceMock) {
    ARAClientMock client = ARAClientMock();
    CHECK_EQUAL(0, client.getNumberOfNetworkInterfaces());

    NetworkInterfaceMock* interface = client.createNewNetworkInterfaceMock();
    AddressMock defaultLocalAddress = AddressMock("DEFAULT");
    CHECK_EQUAL(1, client.getNumberOfNetworkInterfaces());
    CHECK_EQUAL("InterfaceMock1", interface->getName());
    CHECK(interface->getLocalAddress()->equals(&defaultLocalAddress));

    interface = client.createNewNetworkInterfaceMock("192.168.0.1");
    AddressMock expectedLocalAddress = AddressMock("192.168.0.1");
    CHECK_EQUAL(2, client.getNumberOfNetworkInterfaces());
    CHECK_EQUAL("InterfaceMock2", interface->getName());
    CHECK(interface->getLocalAddress()->equals(&expectedLocalAddress));
}

TEST(ARAClientMockTest, testGetNextHop) {
    // this is just an example implementation because the mock simply needs something to be implemented here
    ARAClientMock client = ARAClientMock();
    PacketMock packet = PacketMock();
    RoutingTable* routingTable = client.getRoutingTable();
    Address* destination = packet.getDestination();
    AddressMock node1 = AddressMock("Node 1");
    AddressMock node2 = AddressMock("Node 2");
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    routingTable->update(destination, &node1, &interface, 10);
    routingTable->update(destination, &node2, &interface, 20);

    NextHop* nextHop = client.getNextHop(&packet);
    CHECK(nextHop->getAddress()->equals(&node2));
    CHECK(nextHop->getInterface()->equals(&interface));
}

TEST(ARAClientMockTest, testRememberPacketsThatHaveBeenDeliveredToTheSystem) {
    ARAClientMock client = ARAClientMock();
    PacketMock packet1 = PacketMock("A", "B");
    PacketMock packet2 = PacketMock("B", "C");
    PacketMock packet3 = PacketMock("B", "C");

    LinkedList<const Packet>* deliveredPackets = client.getDeliveredPackets();
    CHECK(deliveredPackets->isEmpty());

    client.deliverToSystem(&packet1);
    LONGS_EQUAL(1, deliveredPackets->size());
    CHECK(deliveredPackets->get(0)->equals(&packet1));

    client.deliverToSystem(&packet2);
    LONGS_EQUAL(2, deliveredPackets->size());
    CHECK(deliveredPackets->get(1)->equals(&packet2));

    client.deliverToSystem(&packet3);
    LONGS_EQUAL(3, deliveredPackets->size());
    CHECK(deliveredPackets->get(2)->equals(&packet3));
}
