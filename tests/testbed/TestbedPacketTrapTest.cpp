/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "Testbed.h"
#include "TestbedPacketTrap.h"

#include "tests/testAPI/mocks/libara/PacketMock.h"
#include "tests/testAPI/mocks/testbed/TestbedARAClientMock.h"
#include "tests/testAPI/mocks/testbed/TestbedNetworkInterfaceMock.h"


/// FIXME: Replace PacketMock with TestbedPacketMock

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedPacketTrapTest){
    TestbedARAClientMock* client;
    std::shared_ptr<TestbedPacketTrap> packetTrap;
    RoutingTable* routingTable;
    //EvaporationPolicy* evaporationPolicy;
    NetworkInterfaceMock* interface;

    void setup(){
        client = new TestbedARAClientMock();
        packetTrap = client->getPacketTrap();
        interface = client->createNewNetworkInterfaceMock();
        routingTable = client->getRoutingTable();
    }

    void teardown(){
        delete client;
    }
};
/*

TEST(TestbedPacketTrapTest, setRoutingTable) {
    TestbedPacket* packet = factory->makeFANT(source, destination, 42);
    CHECK(packet->getSource() == source);
    delete packet;
}

*/


TEST(TestbedPacketTrapTest, trapPacket) {
    Packet* packet = new PacketMock();

    // Check that there is no trapped packet for the packets destination
    CHECK(packetTrap->contains(packet) == false);

    packetTrap->trapPacket(packet);
    CHECK(packetTrap->contains(packet) == true);
}

TEST(TestbedPacketTrapTest, trapMultiplePackets) {
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
}

TEST(TestbedPacketTrapTest, isEmpty) {
    PacketMock packet = PacketMock();

    CHECK(packetTrap->isEmpty());

    packetTrap->trapPacket(&packet);
    CHECK(packetTrap->isEmpty() == false);

    // make the trapped packet deliverable
    AddressPtr destination = packet.getDestination();
    routingTable->update(destination, destination, interface, 10);

    packetTrap->untrapDeliverablePackets(destination);
    CHECK(packetTrap->isEmpty());
}

/*
TEST(TestbedPacketTrapTest, contains) {

}

TEST(TestbedPacketTrapTest, getNumberOfTrappedPackets) {

}

TEST(TestbedPacketTrapTest, getNumberOfTrappedPackets) {

}

TEST(TestbedPacketTrapTest, untrapDeliverablePackets) {

}

TEST(TestbedPacketTrapTest, removePacketsForDestination) {

}
*/

TESTBED_NAMESPACE_END
