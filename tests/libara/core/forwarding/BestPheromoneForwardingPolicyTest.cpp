/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "RoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/libara/ARAClientMock.h"
#include "testAPI/mocks/libara/AddressMock.h"
#include "testAPI/mocks/libara/PacketMock.h"
#include "testAPI/mocks/libara/NetworkInterfaceMock.h"

#include <iostream>
#include <memory>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(BestPheromoneForwardingPolicyTest) {
    ARAClientMock* client;
    BestPheromoneForwardingPolicy* policy;
    RoutingTable* routingTable;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new ARAClientMock();
        routingTable = client->getRoutingTable();
        client->setForwardingPolicy(policy);
        interface = client->createNewNetworkInterfaceMock();
        policy = new BestPheromoneForwardingPolicy(routingTable);
    }

    void teardown() {
        delete client;
        delete policy;
    }
};

TEST(BestPheromoneForwardingPolicyTest, testGetNextHop) {
    PacketMock packet = PacketMock();
    AddressPtr route1 (new AddressMock("A"));
    AddressPtr route2 (new AddressMock("B"));
    AddressPtr route3 (new AddressMock("C"));

    // start the test
    routingTable->update(packet.getDestination(), route1, interface, 1.2);
    routingTable->update(packet.getDestination(), route2, interface, 2.1);
    routingTable->update(packet.getDestination(), route3, interface, 2.3);
    
    NextHop* nextHop = policy->getNextHop(&packet);

    // check if the chosen node matches the node with the highest pheromone value
    CHECK(nextHop->getAddress()->equals(route3));
    CHECK(nextHop->getInterface()->equals(interface));
}

TEST(BestPheromoneForwardingPolicyTest, neverChooseTheSenderOfAPacket) {
    PacketMock packet = PacketMock();
    AddressPtr route1 (new AddressMock("A"));
    AddressPtr route2 (new AddressMock("C"));
    AddressPtr route3 = packet.getSender();

    // start the test
    routingTable->update(packet.getDestination(), route1, interface, 1.2);
    routingTable->update(packet.getDestination(), route2, interface, 2.1);
    routingTable->update(packet.getDestination(), route3, interface, 3.0);

    NextHop* nextHop = policy->getNextHop(&packet);

    // check if the chosen node matches the node with the highest pheromone value
    CHECK(nextHop->getAddress()->equals(route2));
    CHECK(nextHop->getInterface()->equals(interface));
}
