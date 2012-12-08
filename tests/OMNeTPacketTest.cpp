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
#include "OMNeTPacket.h"

using namespace ARA;

SUITE(OMNeTPacketTest) {

    TEST(testCreate) {
        OMNeTAddress source = OMNeTAddress(1);
        OMNeTAddress destination = OMNeTAddress(2);
        unsigned int type = PacketType::DATA;
        int seqNr = 1;
        const char* payload = "Hello ARA World";

        OMNeTPacket packet = OMNeTPacket(&source, &destination, type, seqNr, payload, sizeof(payload));

        CHECK(packet.getSource()->equals(&source));
        CHECK(packet.getDestination()->equals(&destination));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(0, packet.getHopCount());

        CHECK_EQUAL(sizeof(payload), packet.getPayloadLength());
        CHECK_EQUAL(payload, packet.getPayload());
    }

    TEST(testCreateWithHopCount) {
        OMNeTAddress source = OMNeTAddress(1);
        OMNeTAddress destination = OMNeTAddress(2);
        unsigned int type = PacketType::DATA;
        int seqNr = 1;
        const char* payload = "Hello ARA World";
        unsigned int hopCount = 123;

        OMNeTPacket packet = OMNeTPacket(&source, &destination, type, seqNr, payload, sizeof(payload), hopCount);

        CHECK(packet.getSource()->equals(&source));
        CHECK(packet.getDestination()->equals(&destination));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(sizeof(payload), packet.getPayloadLength());
        CHECK_EQUAL(packet.getPayload(), payload);
        CHECK_EQUAL(hopCount, packet.getHopCount());
    }
  }
