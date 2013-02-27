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
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "EvaporationPolicy.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/TimeMock.h"

#include <memory>
#include <deque>

using namespace ARA;

TEST_GROUP(PacketTrapTest) {
    PacketTrap* packetTrap;
    RoutingTable* routingTable;
    EvaporationPolicy* evaporationPolicy;

    void setup() {
        // FIXME do we really need to setup all this stuff here?
        float threshold = 0.2;
        float q = 0.1;
        evaporationPolicy = new LinearEvaporationPolicy(new TimeMock(), new TimeMock(), threshold, q);
        evaporationPolicy->setInterval(10000);
        routingTable = new RoutingTable();
        routingTable->setEvaporationPolicy(evaporationPolicy);
        packetTrap = new PacketTrap(routingTable);
    }

    void teardown() {
        delete packetTrap;
        delete routingTable;
        delete evaporationPolicy;
    }
};

TEST(PacketTrapTest, trapPacket) {
    PacketMock packet = PacketMock();

    // Check that there is no trapped packet for the packets destination
    CHECK(packetTrap->contains(&packet) == false);

    packetTrap->trapPacket(&packet);

    // Now there must be a trapped packet for the packets destination
    CHECK(packetTrap->contains(&packet) == true);
}

TEST(PacketTrapTest, trapMultiplePackets) {
    PacketMock packet1 = PacketMock("A", "B", 1);
    PacketMock packet2 = PacketMock("A", "B", 2);
    PacketMock packet3 = PacketMock("X", "Y", 1);
    PacketMock packet4 = PacketMock("A", "C", 3);

    // start the test
    CHECK(packetTrap->isEmpty());
    packetTrap->trapPacket(&packet1);
    CHECK(packetTrap->contains(&packet1) == true);
    CHECK(packetTrap->contains(&packet2) == false);
    CHECK(packetTrap->contains(&packet3) == false);
    CHECK(packetTrap->contains(&packet4) == false);

    packetTrap->trapPacket(&packet2);
    CHECK(packetTrap->contains(&packet1) == true);
    CHECK(packetTrap->contains(&packet2) == true);
    CHECK(packetTrap->contains(&packet3) == false);
    CHECK(packetTrap->contains(&packet4) == false);

    packetTrap->trapPacket(&packet3);
    CHECK(packetTrap->contains(&packet1) == true);
    CHECK(packetTrap->contains(&packet2) == true);
    CHECK(packetTrap->contains(&packet3) == true);
    CHECK(packetTrap->contains(&packet4) == false);

    packetTrap->trapPacket(&packet4);
    CHECK(packetTrap->contains(&packet1) == true);
    CHECK(packetTrap->contains(&packet2) == true);
    CHECK(packetTrap->contains(&packet3) == true);
    CHECK(packetTrap->contains(&packet4) == true);
}

TEST(PacketTrapTest, getDeliverablePacket) {
    PacketMock trappedPacket = PacketMock();
    std::shared_ptr<Address> someAddress (new AddressMock());
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // Start the test
    std::deque<const Packet*>* deliverablePackets = packetTrap->getDeliverablePackets();
    CHECK(deliverablePackets->empty());   // there is no trapped packet so none can be deliverable
    delete deliverablePackets;

    packetTrap->trapPacket(&trappedPacket);
    deliverablePackets = packetTrap->getDeliverablePackets();
    CHECK(deliverablePackets->empty());   // packet is still not deliverable
    delete deliverablePackets;

    routingTable->update(trappedPacket.getDestination(), someAddress, &interface, 10);
    deliverablePackets = packetTrap->getDeliverablePackets();
    CHECK(deliverablePackets->size() == 1);

    const Packet* deliverablePacket = deliverablePackets->front();
    delete deliverablePackets;

    CHECK(deliverablePacket->getSource()->equals(trappedPacket.getSource()));
    CHECK(deliverablePacket->getDestination()->equals(trappedPacket.getDestination()));
    CHECK_EQUAL(trappedPacket.getType(), deliverablePacket->getType());
    LONGS_EQUAL(trappedPacket.getHopCount(), deliverablePacket->getHopCount());
    LONGS_EQUAL(trappedPacket.getSequenceNumber(), deliverablePacket->getSequenceNumber());
}

TEST(PacketTrapTest, testUntrap) {
    PacketMock packet = PacketMock();

    CHECK(packetTrap->contains(&packet) == false);
    packetTrap->trapPacket(&packet);
    CHECK(packetTrap->contains(&packet) == true);
    packetTrap->untrapPacket(&packet);
    CHECK(packetTrap->contains(&packet) == false);
}

TEST(PacketTrapTest, testIsEmpty) {
    PacketMock packet = PacketMock();

    CHECK(packetTrap->isEmpty());

    packetTrap->trapPacket(&packet);
    CHECK(packetTrap->isEmpty() == false);

    packetTrap->untrapPacket(&packet);
    CHECK(packetTrap->isEmpty());
}

TEST(PacketTrapTest, deleteTrappedPacketsInDestructor) {
    Packet* packet = new PacketMock("source", "destination", 1);
    packetTrap->trapPacket(packet);

    // we need to delete this packet because only a clone of the packet is trapped
    delete packet;

    // when the test finishes, the client will be deleted in teardown()
    // and the packet clone should be deleted as well
}
