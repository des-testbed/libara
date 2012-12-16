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
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "PacketType.h"
#include "Address.h"
#include "LinkedList.h"
#include "Pair.h"

#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

using namespace ARA;

SUITE(AbstractARAClientTest) {

    TEST(testPacketGetsTrappedIfNotDeliverable) {
        ARAClientMock client = ARAClientMock();
        PacketTrap* packetTrap = client.getPacketTrap();
        RoutingTable* routingTable = client.getRoutingTable();
        PacketMock packet = PacketMock();

        CHECK(routingTable->isDeliverable(&packet) == false);
        client.sendPacket(&packet);
        CHECK(packetTrap->contains(&packet));
    }

    TEST(testGeneralBroadCast) {
        ARAClientMock client = ARAClientMock();
        NetworkInterfaceMock interface1 = NetworkInterfaceMock();
        NetworkInterfaceMock interface2 = NetworkInterfaceMock();
        NetworkInterfaceMock interface3 = NetworkInterfaceMock();
        client.addNetworkInterface(&interface1);
        client.addNetworkInterface(&interface2);
        client.addNetworkInterface(&interface3);

        PacketMock packet = PacketMock();
        client.broadCast(&packet);

        CHECK(interface1.hasPacketBeenBroadCasted(&packet) == true);
        CHECK(interface2.hasPacketBeenBroadCasted(&packet) == true);
        CHECK(interface3.hasPacketBeenBroadCasted(&packet) == true);
    }

    TEST(testGetNextSequenceNumber) {
        ARAClientMock client = ARAClientMock();
        for (unsigned int i = 1; i < 100; i++) {
            CHECK_EQUAL(i, client.getNextSequenceNumber());
        }
    }

    TEST(testBroadcastFANTIfPacketNotDeliverable) {
        ARAClientMock client = ARAClientMock();
        RoutingTable* routingTable = client.getRoutingTable();
        NetworkInterfaceMock* interface = client.getDefaultNetworkInterface();
        PacketMock packet = PacketMock();

        CHECK(routingTable->isDeliverable(&packet) == false);
        client.sendPacket(&packet);
        LinkedList<Pair<Packet, Address>>* sentPackets = interface->getSentPackets();
        CHECK_EQUAL(1, sentPackets->size());

        Packet* sentPacket = sentPackets->getFirst()->getLeft();
        Address* recipientOfSentPacket = sentPackets->getFirst()->getRight();

        CHECK(sentPacket->getType() == PacketType::FANT);
        CHECK(recipientOfSentPacket->isBroadCast());
    }

  }
