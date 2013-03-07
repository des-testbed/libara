/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "EvaporationPolicy.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/time/TimeFactoryMock.h"

#include <memory>
#include <deque>

using namespace ARA;

TEST_GROUP(PacketTrapTest) {
    PacketTrap* packetTrap;
    RoutingTable* routingTable;
    EvaporationPolicy* evaporationPolicy;

    void setup() {
        evaporationPolicy = new LinearEvaporationPolicyMock();
        routingTable = new RoutingTable(new TimeFactoryMock());
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
    Packet* packet = new PacketMock();

    // Check that there is no trapped packet for the packets destination
    CHECK(packetTrap->contains(packet) == false);

    packetTrap->trapPacket(packet);

    // Now there must be a trapped packet for the packets destination
    CHECK(packetTrap->contains(packet) == true);
}

TEST(PacketTrapTest, trapMultiplePackets) {
    Packet* packet1 = new PacketMock("A", "B", 1);
    Packet* packet2 = new PacketMock("A", "B", 2);
    Packet* packet3 = new PacketMock("X", "Y", 1);
    Packet* packet4 = new PacketMock("A", "C", 3);

    // start the test
    CHECK(packetTrap->isEmpty());
    packetTrap->trapPacket(packet1);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == false);
    CHECK(packetTrap->contains(packet3) == false);
    CHECK(packetTrap->contains(packet4) == false);

    packetTrap->trapPacket(packet2);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == true);
    CHECK(packetTrap->contains(packet3) == false);
    CHECK(packetTrap->contains(packet4) == false);

    packetTrap->trapPacket(packet3);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == true);
    CHECK(packetTrap->contains(packet3) == true);
    CHECK(packetTrap->contains(packet4) == false);

    packetTrap->trapPacket(packet4);
    CHECK(packetTrap->contains(packet1) == true);
    CHECK(packetTrap->contains(packet2) == true);
    CHECK(packetTrap->contains(packet3) == true);
    CHECK(packetTrap->contains(packet4) == true);
}

TEST(PacketTrapTest, getDeliverablePacket) {
    Packet* trappedPacket = new PacketMock();
    std::shared_ptr<Address> someAddress (new AddressMock());
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // Start the test
    std::deque<Packet*>* deliverablePackets = packetTrap->getDeliverablePackets();
    CHECK(deliverablePackets->empty());   // there is no trapped packet so none can be deliverable
    delete deliverablePackets;

    packetTrap->trapPacket(trappedPacket);
    deliverablePackets = packetTrap->getDeliverablePackets();
    CHECK(deliverablePackets->empty());   // packet is still not deliverable
    delete deliverablePackets;

    routingTable->update(trappedPacket->getDestination(), someAddress, &interface, 10);
    deliverablePackets = packetTrap->getDeliverablePackets();
    CHECK(deliverablePackets->size() == 1);

    Packet* deliverablePacket = deliverablePackets->front();
    delete deliverablePackets;

    CHECK(deliverablePacket == trappedPacket);
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

    // when the test finishes, the client will be deleted in teardown()
    // and the packet clone should be deleted as well
}
