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
#include "testAPI/mocks/AddressMock.h"
#include "Packet.h"
#include "PacketType.h"

using namespace ARA;

SUITE(PacketTest) {

    TEST(testCreate) {
        AddressMock* source = new AddressMock("source");
        AddressMock* destination = new AddressMock("destination");
        unsigned int type = PacketType::FANT;
        int seqNr = 1;

        Packet packet = Packet(source, destination, type, seqNr);

        CHECK(packet.getSource()->equals(source));
        CHECK(packet.getDestination()->equals(destination));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(0, packet.getHopCount());

        CHECK_EQUAL(0, packet.getPayloadLength());
        CHECK(packet.getPayload() == false);
    }

    TEST(testCreateWithPayload) {
        AddressMock* source = new AddressMock("source");
        AddressMock* destination = new AddressMock("destination");
        unsigned int type = PacketType::DATA;
        int seqNr = 2;
        const char* payload = "Hello World!";

        Packet packet = Packet(source, destination, type, seqNr, payload, sizeof(payload));

        CHECK(packet.getSource()->equals(source));
        CHECK(packet.getDestination()->equals(destination));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(0, packet.getHopCount());

        CHECK_EQUAL(sizeof(payload), packet.getPayloadLength());
        CHECK_EQUAL(payload, packet.getPayload());
    }

    TEST(testCreateWithPayloadAndHopCount) {
        Address* source = new AddressMock("source");
        Address* destination = new AddressMock("destination");
        unsigned int type = PacketType::DATA;
        int seqNr = 3;
        const char* payload = "Hello World";
        unsigned int hopCount = 123;

        Packet packet = Packet(source, destination, type, seqNr, payload, sizeof(payload), hopCount);

        CHECK(packet.getSource()->equals(source));
        CHECK(packet.getDestination()->equals(destination));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(sizeof(payload), packet.getPayloadLength());
        CHECK_EQUAL(packet.getPayload(), payload);
        CHECK_EQUAL(hopCount, packet.getHopCount());
    }
  }
