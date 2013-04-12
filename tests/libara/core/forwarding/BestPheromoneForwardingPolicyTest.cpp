/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "RoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "PacketType.h"
#include "Exception.h" 
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"

#include <iostream>
#include <memory>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(BestPheromoneForwardingPolicyTest) {
    BestPheromoneForwardingPolicy* policy;
    EvaporationPolicy* evaporationPolicy;
    RoutingTable* routingTable;
    NetworkInterfaceMock* interface;

    void setup() {
        policy = new BestPheromoneForwardingPolicy();
        evaporationPolicy = new ExponentialEvaporationPolicyMock();
        routingTable = new RoutingTable();
        routingTable->setEvaporationPolicy(evaporationPolicy);
        interface = new NetworkInterfaceMock();
    }

    void teardown() {
        delete routingTable;
        delete evaporationPolicy;
        delete policy;
        delete interface;
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
    
    NextHop* nextHop = policy->getNextHop(&packet, routingTable);

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

    NextHop* nextHop = policy->getNextHop(&packet, routingTable);

    // check if the chosen node matches the node with the highest pheromone value
    CHECK(nextHop->getAddress()->equals(route2));
    CHECK(nextHop->getInterface()->equals(interface));
}
