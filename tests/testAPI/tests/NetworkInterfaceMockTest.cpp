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
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "LinkedList.h"
#include "Pair.h"

using namespace ARA;

SUITE(NetworkInterfaceMockTest) {

    TEST(testMockRemembersSentPackets) {
        NetworkInterfaceMock mock = NetworkInterfaceMock();
        AddressMock recipient1 = AddressMock();
        AddressMock recipient2 = AddressMock();
        PacketMock packet1 = PacketMock();
        PacketMock packet2 = PacketMock();
        PacketMock packet3 = PacketMock();

        mock.send(&packet1, &recipient1);
        mock.send(&packet2, &recipient2);
        mock.send(&packet3, &recipient1);

        LinkedList<Pair<Packet, Address>>* sendPackets = mock.getSentPackets();

        CHECK(sendPackets->get(0)->getLeft() == &packet1);
        CHECK(sendPackets->get(0)->getRight() == &recipient1);

        CHECK(sendPackets->get(1)->getLeft() == &packet2);
        CHECK(sendPackets->get(1)->getRight() == &recipient2);

        CHECK(sendPackets->get(2)->getLeft() == &packet3);
        CHECK(sendPackets->get(2)->getRight() == &recipient1);
    }

  }
