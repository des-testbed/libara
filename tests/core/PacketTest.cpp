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
#include <cstring>
#include "testAPI/mocks/AddressMock.h"
#include "Packet.h"
#include "PacketType.h"

using namespace ARA;

SUITE(PacketTest) {

    TEST(testCreate) {
        AddressMock* source = new AddressMock("source");
        AddressMock* destination = new AddressMock("destination");
        AddressMock* sender = new AddressMock("sender");    // This is the node from which the packet has actually been received
        unsigned int type = PacketType::FANT;
        int seqNr = 1;

        Packet packet = Packet(source, destination, sender, type, seqNr);

        CHECK(packet.getSource()->equals(source));
        CHECK(packet.getDestination()->equals(destination));
        CHECK(packet.getSender()->equals(sender));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(0, packet.getHopCount());

        CHECK_EQUAL(0, packet.getPayloadLength());
        CHECK(packet.getPayload() == false);
    }

    TEST(testCreateWithPayload) {
        AddressMock* source = new AddressMock("source");
        AddressMock* destination = new AddressMock("destination");
        AddressMock* sender = new AddressMock("sender");
        unsigned int type = PacketType::DATA;
        int seqNr = 2;
        const char* payload = "Hello World!";

        Packet packet = Packet(source, destination, sender, type, seqNr, payload, strlen(payload));

        CHECK(packet.getSource()->equals(source));
        CHECK(packet.getDestination()->equals(destination));
        CHECK(packet.getSender()->equals(sender));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(0, packet.getHopCount());

        CHECK_EQUAL(strlen(payload), packet.getPayloadLength());
        CHECK_EQUAL(payload, packet.getPayload());
    }

    TEST(testCreateWithPayloadAndHopCount) {
        Address* source = new AddressMock("source");
        Address* destination = new AddressMock("destination");
        AddressMock* sender = new AddressMock("sender");
        unsigned int type = PacketType::DATA;
        int seqNr = 3;
        const char* payload = "Hello World";
        unsigned int hopCount = 123;

        Packet packet = Packet(source, destination, sender, type, seqNr, payload, strlen(payload), hopCount);

        CHECK(packet.getSource()->equals(source));
        CHECK(packet.getDestination()->equals(destination));
        CHECK(packet.getSender()->equals(sender));
        CHECK_EQUAL(type, packet.getType());
        CHECK_EQUAL(seqNr, packet.getSequenceNumber());
        CHECK_EQUAL(strlen(payload), packet.getPayloadLength());
        CHECK_EQUAL(payload, packet.getPayload());
        CHECK_EQUAL(hopCount, packet.getHopCount());
    }

    TEST(testCreateFANT) {
       Address* source = new AddressMock("source");
       Address* destination = new AddressMock("destination");
       AddressMock* sender = new AddressMock("sender");
       unsigned int type = PacketType::DATA;
       int seqNr = 3;
       const char* payload = "Hello World";
       unsigned int hopCount = 123;

       Packet packet = Packet(source, destination, sender, type, seqNr, payload, strlen(payload), hopCount);
       unsigned int newSequenceNumber = 242342;
       Packet* fant = packet.createFANT(newSequenceNumber);

       CHECK(fant->getSource()->equals(source));
       CHECK(fant->getDestination()->equals(destination));
       // The sender of a FANT will be determined when it is actually send over a network interface
       CHECK_EQUAL(PacketType::FANT, fant->getType());
       CHECK_EQUAL(newSequenceNumber, fant->getSequenceNumber());
       CHECK_EQUAL(0, fant->getPayloadLength());
       CHECK_EQUAL(hopCount, fant->getHopCount());

       delete fant;
    }

    TEST(testClone) {
       Address* source = new AddressMock("source");
       Address* destination = new AddressMock("destination");
       AddressMock* sender = new AddressMock("sender");
       unsigned int type = PacketType::DATA;
       int seqNr = 3;
       const char* payload = "Hello World";
       unsigned int hopCount = 123;

       Packet packet = Packet(source, destination, sender, type, seqNr, payload, strlen(payload), hopCount);
       Packet* clone = packet.clone();

       CHECK(clone->getSource()->equals(source));
       CHECK(clone->getDestination()->equals(destination));
       CHECK(clone->getSender()->equals(sender));
       CHECK_EQUAL(type, clone->getType());
       CHECK_EQUAL(seqNr, clone->getSequenceNumber());
       CHECK_EQUAL(strlen(payload), clone->getPayloadLength());
       CHECK_EQUAL(payload, clone->getPayload());
       CHECK_EQUAL(hopCount, clone->getHopCount());
       CHECK(packet.equals(clone));

       delete clone;
    }

    TEST(testSetHopCount) {
       Address* source = new AddressMock("source");
       Address* destination = new AddressMock("destination");
       Address* sender = new AddressMock("sender");
       unsigned int type = PacketType::DATA;
       int seqNr = 3;
       const char* payload = "Hello World";
       unsigned int hopCount = 123;

       Packet packet = Packet(source, destination, sender, type, seqNr, payload, strlen(payload), hopCount);
       CHECK_EQUAL(hopCount, packet.getHopCount());

       unsigned int newHopCount = 124;
       packet.setHopCount(newHopCount);
       CHECK_EQUAL(newHopCount, packet.getHopCount());
    }

    TEST(testEquals) {
       unsigned int dataPacket = PacketType::DATA;
       const char* payload = "Hello World";

       Packet packet            = Packet(new AddressMock("Source1"), new AddressMock("Destination"), new AddressMock("Sender"), dataPacket, 1, payload, strlen(payload));
       Packet samePacket        = Packet(new AddressMock("Source1"), new AddressMock("Destination"), new AddressMock("Sender"), dataPacket, 1, payload, strlen(payload));
       Packet nextSeqPacket     = Packet(new AddressMock("Source2"), new AddressMock("Destination"), new AddressMock("Sender"), dataPacket, 2, payload, strlen(payload));
       Packet otherSourcePacket = Packet(new AddressMock("Source2"), new AddressMock("Destination"), new AddressMock("Sender"), dataPacket, 1, payload, strlen(payload));
       Packet otherPacket       = Packet(new AddressMock("Source2"), new AddressMock("Destination"), new AddressMock("Sender"), dataPacket, 3, payload, strlen(payload));

       CHECK(packet.equals(&packet));
       CHECK(packet.equals(&samePacket));
       CHECK(packet.equals(&nextSeqPacket) == false);
       CHECK(packet.equals(&otherSourcePacket) == false);
       CHECK(packet.equals(&otherPacket) == false);
    }
}
