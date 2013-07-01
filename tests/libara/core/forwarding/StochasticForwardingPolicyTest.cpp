/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "RoutingTable.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/StochasticForwardingPolicyMock.h"

#include <iostream>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(StochasticForwardingPolicyTest) {
    ARAClientMock* client;
    EvaporationPolicy* evaporationPolicy;
    RoutingTable* routingTable;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new ARAClientMock();
        routingTable = client->getRoutingTable();
        evaporationPolicy = routingTable->getEvaporationPolicy();
        interface = client->createNewNetworkInterfaceMock();
    }

    void teardown() {
        delete client;
    }
};

TEST(StochasticForwardingPolicyTest, testGetNextHop) {
    PacketMock packet = PacketMock();
    AddressPtr route1 (new AddressMock("A"));
    AddressPtr route2 (new AddressMock("B"));

    routingTable->update(packet.getDestination(), route1, interface, 1.2);
    routingTable->update(packet.getDestination(), route2, interface, 2.1);

    unsigned int seed = 42;
    StochasticForwardingPolicyMock policy = StochasticForwardingPolicyMock(seed);

    NextHop* nextHop = policy.getNextHop(&packet, routingTable);
    CHECK(nextHop->getAddress()->equals(route1));
}

TEST(StochasticForwardingPolicyTest, stochasticBehaviour) {
    PacketMock packet = PacketMock();
    AddressPtr route1 (new AddressMock("A"));
    AddressPtr route2 (new AddressMock("B"));
    AddressPtr route3 (new AddressMock("C"));

    routingTable->update(packet.getDestination(), route1, interface, 3.0);
    routingTable->update(packet.getDestination(), route2, interface, 6.0);
    routingTable->update(packet.getDestination(), route3, interface, 1.0);

    int nrOfIterations = 10000;
    int nrOfTimesRoute1IsChosen = 0;
    int nrOfTimesRoute2IsChosen = 0;
    int nrOfTimesRoute3IsChosen = 0;

    StochasticForwardingPolicyMock policy = StochasticForwardingPolicyMock();
    for (int i = 0; i < nrOfIterations; i++) {
        NextHop* nextHop = policy.getNextHop(&packet, routingTable);
        if(nextHop->getAddress()->equals(route1)) {
            nrOfTimesRoute1IsChosen++;
        }
        else if(nextHop->getAddress()->equals(route2)) {
            nrOfTimesRoute2IsChosen++;
        }
        else if(nextHop->getAddress()->equals(route3)) {
            nrOfTimesRoute3IsChosen++;
        }
    }

    unsigned int allowedDeviation = nrOfIterations/20; // allow a deviation of +- 5%
    CHECK(nrOfTimesRoute1IsChosen > (nrOfIterations*0.3 - allowedDeviation));
    CHECK(nrOfTimesRoute1IsChosen < (nrOfIterations*0.3 + allowedDeviation));

    CHECK(nrOfTimesRoute2IsChosen > (nrOfIterations*0.6 - allowedDeviation));
    CHECK(nrOfTimesRoute2IsChosen < (nrOfIterations*0.6 + allowedDeviation));

    CHECK(nrOfTimesRoute3IsChosen > (nrOfIterations*0.1 - allowedDeviation));
    CHECK(nrOfTimesRoute3IsChosen < (nrOfIterations*0.1 + allowedDeviation));
}

TEST(StochasticForwardingPolicyTest, neverChooseTheSenderOfAPacket) {
    PacketMock packet = PacketMock();
    AddressPtr route1 (new AddressMock("A"));
    AddressPtr route2 (new AddressMock("B"));
    AddressPtr route3 (new AddressMock("C"));

    routingTable->update(packet.getDestination(), route1, interface, 3.0);
    routingTable->update(packet.getDestination(), route2, interface, 6.0);
    routingTable->update(packet.getDestination(), packet.getSender(), interface, 5.0);

    int nrOfIterations = 100;

    StochasticForwardingPolicyMock policy = StochasticForwardingPolicyMock();
    for (int i = 0; i < nrOfIterations; i++) {
        NextHop* nextHop = policy.getNextHop(&packet, routingTable);
        CHECK(nextHop->getAddress()->equals(packet.getSender()) == false);
    }
}
