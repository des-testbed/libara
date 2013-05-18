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
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"

#include <memory>
#include <deque>
#include <algorithm>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(PacketTrapTest) {
    PacketTrap* packetTrap;
    RoutingTable* routingTable;
    EvaporationPolicy* evaporationPolicy;

    void setup() {
        evaporationPolicy = new ExponentialEvaporationPolicyMock();
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

TEST(PacketTrapTest, untrapDeliverablePackets) {
    Packet* trappedPacket1 = new PacketMock("src", "dest");
    Packet* trappedPacket2 = new PacketMock("src", "foo");
    AddressPtr destination1 = trappedPacket1->getDestination();
    AddressPtr destination2 = trappedPacket2->getDestination();
    AddressPtr someAddress (new AddressMock("bar"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // Start the test
    PacketQueue deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.empty());   // there is no trapped packet so none can be deliverable

    packetTrap->trapPacket(trappedPacket1);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.empty());   // packet is still not deliverable
    BYTES_EQUAL(1, packetTrap->getNumberOfTrappedPackets());

    // update the route to some other destination which should not make this destination deliverable
    routingTable->update(someAddress, someAddress, &interface, 8);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.empty());
    BYTES_EQUAL(1, packetTrap->getNumberOfTrappedPackets());

    // trap another packet for a different destination
    packetTrap->trapPacket(trappedPacket2);
    BYTES_EQUAL(2, packetTrap->getNumberOfTrappedPackets());

    // now update the route to our wanted destination which should make it deliverable
    routingTable->update(trappedPacket1->getDestination(), someAddress, &interface, 10);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination1);
    CHECK(deliverablePackets.size() == 1);

    // the packet should have been removed from the packet trap
    BYTES_EQUAL(1, packetTrap->getNumberOfTrappedPackets());
    Packet* deliverablePacket = deliverablePackets.front();
    CHECK(deliverablePacket == trappedPacket1);
    delete deliverablePacket;

    // also get the other trapped packet
    routingTable->update(trappedPacket2->getDestination(), someAddress, &interface, 10);
    deliverablePackets = packetTrap->untrapDeliverablePackets(destination2);
    CHECK(deliverablePackets.size() == 1);

    // the packet should have been removed from the packet trap
    BYTES_EQUAL(0, packetTrap->getNumberOfTrappedPackets());
    deliverablePacket = deliverablePackets.front();
    CHECK(deliverablePacket == trappedPacket2);
    delete deliverablePacket;
}

TEST(PacketTrapTest, testIsEmpty) {
    PacketMock packet = PacketMock();

    CHECK(packetTrap->isEmpty());

    packetTrap->trapPacket(&packet);
    CHECK(packetTrap->isEmpty() == false);

    // make the trapped packet deliverable
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr destination = packet.getDestination();
    routingTable->update(destination, destination, &interface, 10);

    packetTrap->untrapDeliverablePackets(destination);
    CHECK(packetTrap->isEmpty());
}

TEST(PacketTrapTest, deleteTrappedPacketsInDestructor) {
    Packet* packet = new PacketMock("source", "destination", 1);
    packetTrap->trapPacket(packet);

    // when the test finishes, the client will be deleted in teardown()
    // and the packet clone should be deleted as well
}

TEST(PacketTrapTest, removePacketsForDestination) {
    AddressPtr someAddress = AddressPtr(new AddressMock("foo"));
    Packet* packet1 = new PacketMock("A", "B", 1);
    Packet* packet2 = new PacketMock("A", "B", 2);
    Packet* packet3 = new PacketMock("X", "Y", 1);
    Packet* packet4 = new PacketMock("A", "C", 3);

    PacketQueue removedPackets = packetTrap->removePacketsForDestination(someAddress);
    CHECK(removedPackets.empty());

    packetTrap->trapPacket(packet1);
    packetTrap->trapPacket(packet2);
    packetTrap->trapPacket(packet3);
    packetTrap->trapPacket(packet4);

    removedPackets = packetTrap->removePacketsForDestination(packet1->getDestination());

    // check that packet 1 and 2 are in the returned list (damn std api suckz)
    BYTES_EQUAL(2, removedPackets.size());
    CHECK(std::find(removedPackets.begin(), removedPackets.end(), packet1) != removedPackets.end());
    CHECK(std::find(removedPackets.begin(), removedPackets.end(), packet2) != removedPackets.end());

    // check that the packets are no longer in the trap
    CHECK_FALSE(packetTrap->contains(packet1));
    CHECK_FALSE(packetTrap->contains(packet2));

    // check that the other packets are still in the trap
    CHECK_TRUE(packetTrap->contains(packet3));
    CHECK_TRUE(packetTrap->contains(packet4));

    delete packet1;
    delete packet2;
}
