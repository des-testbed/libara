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
#include <cstring>
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"

using namespace ARA;

TEST_GROUP(PacketMockTest) {};

TEST(PacketMockTest, testConstructor) {
    PacketMock mock = PacketMock("Source", "Destination", 123, 0);

    AddressMock wantedSource = AddressMock("Source");
    AddressMock wantedDestination = AddressMock("Destination");
    AddressMock wantedSender = wantedSource;

    CHECK(mock.getSource()->equals(&wantedSource));
    CHECK(mock.getDestination()->equals(&wantedDestination));
    CHECK(mock.getSender()->equals(&wantedSender));
    CHECK_EQUAL(123, mock.getSequenceNumber());
    CHECK_EQUAL(0, mock.getHopCount());
    STRCMP_EQUAL("Hello World", mock.getPayload());
    CHECK_EQUAL(strlen("Hello World"), mock.getPayloadLength());
}

TEST(PacketMockTest, testSetSender) {
    PacketMock mock = PacketMock("A", "B");
    CHECK(mock.getSender()->equals(mock.getSource()));

    AddressMock* newSender = new AddressMock("C");  // the PacketMock will take care of deleting this object
    mock.setSender(newSender);

    CHECK(mock.getSender()->equals(newSender));
}

TEST(PacketMockTest, testClone) {
    PacketMock packet = PacketMock();
    Packet* clone = packet.clone();

   CHECK(clone->getSource()->equals(packet.getSource()));
   CHECK(clone->getDestination()->equals(packet.getDestination()));
   CHECK(clone->getSender()->equals(packet.getSender()));
   CHECK_EQUAL(packet.getType(), clone->getType());
   CHECK_EQUAL(packet.getSequenceNumber(), clone->getSequenceNumber());
   CHECK_EQUAL(packet.getPayloadLength(), clone->getPayloadLength());
   CHECK_EQUAL(packet.getPayload(), clone->getPayload());
   CHECK_EQUAL(packet.getHopCount(), clone->getHopCount());
   CHECK(packet.equals(clone));

   delete clone;
}
