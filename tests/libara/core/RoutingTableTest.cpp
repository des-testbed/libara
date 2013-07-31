/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "RoutingTable.h"
#include "ExponentialEvaporationPolicy.h"
#include "RoutingTableEntry.h"
#include "PacketType.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/time/TimeMock.h"
#include "testAPI/mocks/ARAClientMock.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableTest) {
    ARAClientMock* client;
    RoutingTable* routingTable;
    ExponentialEvaporationPolicyMock* evaporationPolicy;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new ARAClientMock();
        routingTable = client->getRoutingTable();
        evaporationPolicy = (ExponentialEvaporationPolicyMock*) routingTable->getEvaporationPolicy();
        interface = client->createNewNetworkInterfaceMock();
    }

    void teardown() {
        delete client;
    }
};

TEST(RoutingTableTest, getPossibleNextHopsReturnsEmptyList) {
    PacketMock packet = PacketMock();
    std::deque<RoutingTableEntry*> list = routingTable->getPossibleNextHops(&packet);
    CHECK(list.empty());
}

TEST(RoutingTableTest, packetWithUnregisteredAddressIsNotDeliverable) {
    PacketMock packet = PacketMock();
    CHECK(routingTable->isDeliverable(&packet) == false);
}

TEST(RoutingTableTest, updateRoutingTable) {
    PacketMock packet = PacketMock();
    AddressPtr destination = packet.getDestination();
    AddressPtr nextHop (new AddressMock("nextHop"));
    float pheromoneValue = 123.456;

    CHECK(routingTable->isDeliverable(&packet) == false);
    routingTable->update(destination, nextHop, interface, pheromoneValue);

    CHECK(routingTable->isDeliverable(&packet));
    std::deque<RoutingTableEntry*> nextHops = routingTable->getPossibleNextHops(&packet);
    CHECK(nextHops.size() == 1);
    RoutingTableEntry* possibleHop = nextHops.front();
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());
}

TEST(RoutingTableTest, overwriteExistingEntryWithUpdate) {
    PacketMock packet = PacketMock();
    AddressPtr destination = packet.getDestination();
    AddressPtr nextHop (new AddressMock("nextHop"));
    float pheromoneValue = 123.456;

    // first we register a route to a destination the first time
    routingTable->update(destination, nextHop, interface, pheromoneValue);

    CHECK(routingTable->isDeliverable(&packet));
    std::deque<RoutingTableEntry*> nextHops = routingTable->getPossibleNextHops(&packet);
    BYTES_EQUAL(1, nextHops.size());
    RoutingTableEntry* possibleHop = nextHops.front();
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());

    // now we want to update the pheromone value of this route
    routingTable->update(destination, nextHop, interface, 42);
    nextHops = routingTable->getPossibleNextHops(&packet);
    BYTES_EQUAL(1, nextHops.size());
    possibleHop = nextHops.front();
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(42, possibleHop->getPheromoneValue());
}

TEST(RoutingTableTest, getPossibleNextHops) {
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination1 (new AddressMock("Destination1"));
    AddressPtr destination2 (new AddressMock("Destination2"));

    AddressPtr nextHop1a (new AddressMock("nextHop1a"));
    AddressPtr nextHop1b (new AddressMock("nextHop1b"));
    AddressPtr nextHop2 (new AddressMock("nextHop2"));
    AddressPtr nextHop3 (new AddressMock("nextHop3"));
    AddressPtr nextHop4 (new AddressMock("nextHop4"));
    NetworkInterfaceMock interface1 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface2 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface3 = NetworkInterfaceMock(client);

    float pheromoneValue1a = 1;
    float pheromoneValue1b = 5;
    float pheromoneValue2 = 2.3;
    float pheromoneValue3 = 4;
    float pheromoneValue4 = 2;

    routingTable->update(destination1, nextHop1a, &interface1, pheromoneValue1a);
    routingTable->update(destination1, nextHop1b, &interface1, pheromoneValue1b);
    routingTable->update(destination1, nextHop2, &interface2, pheromoneValue2);

    routingTable->update(destination2, nextHop3, &interface3, pheromoneValue3);
    routingTable->update(destination2, nextHop4, &interface1, pheromoneValue4);

    Packet packet1 = Packet(sourceAddress, destination1, sourceAddress, PacketType::DATA, 123, 10);
    Packet packet2 = Packet(sourceAddress, destination2, sourceAddress, PacketType::DATA, 124, 10);

    std::deque<RoutingTableEntry*> nextHopsForDestination1 = routingTable->getPossibleNextHops(&packet1);
    BYTES_EQUAL(3, nextHopsForDestination1.size());
    for (unsigned int i = 0; i < nextHopsForDestination1.size(); i++) {
        RoutingTableEntry* possibleHop = nextHopsForDestination1.at(i);
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop1a)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1a, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop1b)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1b, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop2)) {
            CHECK_EQUAL(&interface2, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue2, possibleHop->getPheromoneValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop1a, nextHop1b or nextHop2
        }
    }

    std::deque<RoutingTableEntry*> nextHopsForDestination2 = routingTable->getPossibleNextHops(&packet2);
    BYTES_EQUAL(2, nextHopsForDestination2.size());
    for (unsigned int i = 0; i < nextHopsForDestination2.size(); i++) {
        RoutingTableEntry* possibleHop = nextHopsForDestination2.at(i);
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop3)) {
            CHECK_EQUAL(&interface3, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue3, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop4)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue4, possibleHop->getPheromoneValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop3 or nextHop4
        }
    }
}

TEST(RoutingTableTest, getPheromoneValue) {
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nextHopAddress (new AddressMock("nextHop"));

    // Should be zero because there is no known route to this destination
    LONGS_EQUAL(0, routingTable->getPheromoneValue(destination, nextHopAddress, interface));

    routingTable->update(destination, nextHopAddress, interface, 123);
    LONGS_EQUAL(123, routingTable->getPheromoneValue(destination, nextHopAddress, interface));
}

TEST(RoutingTableTest, removeEntry) {
    AddressPtr destination (new AddressMock("Destination"));

    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));

    routingTable->update(destination, nodeA, interface, 2.5);
    routingTable->update(destination, nodeB, interface, 2.5);
    routingTable->update(destination, nodeC, interface, 2.5);

    // start the test
    BYTES_EQUAL(3, routingTable->getTotalNumberOfEntries());
    routingTable->removeEntry(destination, nodeB, interface);
    BYTES_EQUAL(2, routingTable->getTotalNumberOfEntries());

    for (unsigned int i = 0; i < 2; i++) {
        RoutingTableEntryTupel tupel = routingTable->getEntryAt(i);
        if(tupel.entry->getAddress()->equals(nodeB)) {
            FAIL("The deleted hop should not longer be in the list of possible next hops");
        }
    }
}

TEST(RoutingTableTest, evaporatePheromones) {
    AddressPtr destination (new AddressMock("Destination"));

    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    float pheromoneValueA = 2.5f;
    float pheromoneValueB = 3.8f;
    float pheromoneValueC = 0.2f;

    routingTable->update(destination, nodeA, interface, pheromoneValueA);
    routingTable->update(destination, nodeB, interface, pheromoneValueB);
    routingTable->update(destination, nodeC, interface, pheromoneValueC);

    // no time has passed so nothing is evaporated
    routingTable->triggerEvaporation();
    CHECK_EQUAL(pheromoneValueA, routingTable->getPheromoneValue(destination, nodeA, interface));
    CHECK_EQUAL(pheromoneValueB, routingTable->getPheromoneValue(destination, nodeB, interface));
    CHECK_EQUAL(pheromoneValueC, routingTable->getPheromoneValue(destination, nodeC, interface));

    // let some time pass to trigger the evaporation
    TimeMock::letTimePass(evaporationPolicy->getTimeInterval());
    routingTable->triggerEvaporation();
    float evaporationFactor = evaporationPolicy->getEvaporationFactor();
    DOUBLES_EQUAL(pheromoneValueA * evaporationFactor, routingTable->getPheromoneValue(destination, nodeA, interface), 0.00001);
    DOUBLES_EQUAL(pheromoneValueB * evaporationFactor, routingTable->getPheromoneValue(destination, nodeB, interface), 0.00001);

    // pheromoneValueC should be well below the threshold and therefore be zero
    CHECK_EQUAL(0.0, routingTable->getPheromoneValue(destination, nodeC, interface));
}

TEST(RoutingTableTest, exists) {
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));

    // start the test
    CHECK_FALSE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeC, interface));

    routingTable->update(destination, nodeA, interface, 2.5);
    CHECK_TRUE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeC, interface));

    routingTable->update(destination, nodeC, interface, 3.7);
    CHECK_TRUE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_TRUE(routingTable->exists(destination, nodeC, interface));

    routingTable->removeEntry(destination, nodeA, interface);
    CHECK_FALSE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_TRUE(routingTable->exists(destination, nodeC, interface));

    routingTable->removeEntry(destination, nodeC, interface);
    CHECK_FALSE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeC, interface));
}

TEST(RoutingTableTest, removeAllEntries) {
    AddressPtr destination (new AddressMock("Destination"));

    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));

    routingTable->update(destination, nodeA, interface, 2.5);
    routingTable->update(destination, nodeB, interface, 2.5);
    routingTable->update(destination, nodeC, interface, 2.5);

    // sanity check
    CHECK(routingTable->isDeliverable(destination) == true);

    // start the test
    routingTable->removeEntry(destination, nodeB, interface);
    CHECK_TRUE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_TRUE(routingTable->exists(destination, nodeC, interface));

    routingTable->removeEntry(destination, nodeA, interface);
    CHECK_FALSE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_TRUE(routingTable->exists(destination, nodeC, interface));

    routingTable->removeEntry(destination, nodeC, interface);
    CHECK_FALSE(routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeC, interface));

    BYTES_EQUAL(0, routingTable->getTotalNumberOfEntries());
    CHECK(routingTable->isDeliverable(destination) == false);
}

TEST(RoutingTableTest, isNewRoute) {
    AddressPtr source (new AddressMock("Source"));
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));

    // start the test
    CHECK(routingTable->isNewRoute(destination, nodeA, interface) == true);
    CHECK(routingTable->isNewRoute(destination, nodeB, interface) == true);

    routingTable->update(destination, nodeA, interface, 2.5);
    CHECK(routingTable->isNewRoute(destination, nodeA, interface) == false);
    CHECK(routingTable->isNewRoute(destination, nodeB, interface) == true);

    routingTable->update(destination, nodeB, interface, 4.1);
    CHECK(routingTable->isNewRoute(destination, nodeA, interface) == false);
    CHECK(routingTable->isNewRoute(destination, nodeB, interface) == false);

    routingTable->removeEntry(destination, nodeA, interface);
    CHECK(routingTable->isNewRoute(destination, nodeA, interface) == true);
    CHECK(routingTable->isNewRoute(destination, nodeB, interface) == false);

    routingTable->removeEntry(destination, nodeB, interface);
    CHECK(routingTable->isNewRoute(destination, nodeA, interface) == true);
    CHECK(routingTable->isNewRoute(destination, nodeB, interface) == true);
}

/**
 * In this test we check that RoutingTable::isDeliverable(Packet packet)
 * returns false if the only known route do a packet.destination leads
 * over the packet.sender.
 *
 */
TEST(RoutingTableTest, packetIsNotDeliverableIfOnlyRouteLeadsBackToTheSender) {
    PacketMock packet = PacketMock();
    routingTable->update(packet.getDestination(), packet.getSender(), interface, 10.0);

    CHECK_FALSE(routingTable->isDeliverable(&packet));
}

TEST(RoutingTableTest, getTotalNumberOfEntries) {
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));

    // empty at the beginning
    BYTES_EQUAL(0, routingTable->getTotalNumberOfEntries());

    // one known route to A
    routingTable->update(nodeA, nodeB, interface, 10);
    BYTES_EQUAL(1, routingTable->getTotalNumberOfEntries());

    // two known routes to A
    routingTable->update(nodeA, nodeC, interface, 20);
    BYTES_EQUAL(2, routingTable->getTotalNumberOfEntries());

    // and another route to B
    routingTable->update(nodeB, nodeC, interface, 1.2);
    BYTES_EQUAL(3, routingTable->getTotalNumberOfEntries());
}

TEST(RoutingTableTest, tableEntriesAreDeletedIfEvaporationReachesZero) {
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));

    // empty at the beginning
    routingTable->triggerEvaporation();
    BYTES_EQUAL(0, routingTable->getTotalNumberOfEntries());

    // create some routes
    routingTable->update(nodeA, nodeB, interface, 2);
    routingTable->update(nodeA, nodeC, interface, 2);
    routingTable->update(nodeB, nodeC, interface, 2);
    BYTES_EQUAL(3, routingTable->getTotalNumberOfEntries());

    // now let them all evaporate
    TimeMock::letTimePass(100000);
    routingTable->triggerEvaporation();
    BYTES_EQUAL(0, routingTable->getTotalNumberOfEntries());
}

TEST(RoutingTableTest, falselyDeleteLastEntryBug) {
    AddressPtr destination (new AddressMock("dest"));
    AddressPtr nextHop (new AddressMock("A"));
    AddressPtr anotherAddress (new AddressMock("B"));

    routingTable->update(destination, nextHop, interface, 2);

    CHECK(routingTable->exists(destination, nextHop, interface));
    routingTable->removeEntry(destination, anotherAddress, interface);
    CHECK(routingTable->exists(destination, nextHop, interface));
}

TEST(RoutingTableTest, getPossibleNextHopsForDestination) {
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination1 (new AddressMock("Destination1"));
    AddressPtr destination2 (new AddressMock("Destination2"));

    AddressPtr nextHop1a (new AddressMock("nextHop1a"));
    AddressPtr nextHop1b (new AddressMock("nextHop1b"));
    AddressPtr nextHop2 (new AddressMock("nextHop2"));
    AddressPtr nextHop3 (new AddressMock("nextHop3"));
    AddressPtr nextHop4 (new AddressMock("nextHop4"));
    NetworkInterfaceMock interface1 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface2 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface3 = NetworkInterfaceMock(client);

    float pheromoneValue1a = 1;
    float pheromoneValue1b = 5;
    float pheromoneValue2 = 2.3;
    float pheromoneValue3 = 4;
    float pheromoneValue4 = 2;

    routingTable->update(destination1, nextHop1a, &interface1, pheromoneValue1a);
    routingTable->update(destination1, nextHop1b, &interface1, pheromoneValue1b);
    routingTable->update(destination1, nextHop2, &interface2, pheromoneValue2);

    routingTable->update(destination2, nextHop3, &interface3, pheromoneValue3);
    routingTable->update(destination2, nextHop4, &interface1, pheromoneValue4);

    std::deque<RoutingTableEntry*> nextHopsForDestination1 = routingTable->getPossibleNextHops(destination1);
    BYTES_EQUAL(3, nextHopsForDestination1.size());
    for (unsigned int i = 0; i < nextHopsForDestination1.size(); i++) {
        RoutingTableEntry* possibleHop = nextHopsForDestination1.at(i);
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop1a)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1a, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop1b)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1b, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop2)) {
            CHECK_EQUAL(&interface2, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue2, possibleHop->getPheromoneValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop1a, nextHop1b or nextHop2
        }
    }

    std::deque<RoutingTableEntry*> nextHopsForDestination2 = routingTable->getPossibleNextHops(destination2);
    BYTES_EQUAL(2, nextHopsForDestination2.size());
    for (unsigned int i = 0; i < nextHopsForDestination2.size(); i++) {
        RoutingTableEntry* possibleHop = nextHopsForDestination2.at(i);
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop3)) {
            CHECK_EQUAL(&interface3, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue3, possibleHop->getPheromoneValue());
        }
        else if(hopAddress->equals(nextHop4)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue4, possibleHop->getPheromoneValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop3 or nextHop4
        }
    }
}

/**
 * In this test we check if the routing table correctly returns a list of routing table entries
 * which lead over a specified hop.
 *
 * Test setup:                   | Description:
 *                ┌--->(dest1)   |   * We are testing from the perspective of (A)
 * (...)---(A)---(B)-->(dest2)   |
 *          |     └--->(dest3)   |
 *          |             ↑      |
 *          └--->(C)--->--┘      |
 */
TEST(RoutingTableTest, getAllRoutesThatLeadOverSpecificNextHop) {
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr someUnknownNode (new AddressMock("X"));
    AddressPtr dest1 (new AddressMock("dest1"));
    AddressPtr dest2 (new AddressMock("dest2"));
    AddressPtr dest3 (new AddressMock("dest3"));

    routingTable->update(dest1, nodeB, interface, 10);
    routingTable->update(dest2, nodeB, interface, 10);
    routingTable->update(dest3, nodeB, interface, 10);
    routingTable->update(dest3, nodeC, interface, 10);

    std::deque<RoutingTableEntryTupel> entriesOverB = routingTable->getAllRoutesThatLeadOver(nodeB);
    BYTES_EQUAL(3, entriesOverB.size());
    for (auto& entry: entriesOverB) {
        CHECK(entry.destination->equals(dest1) || entry.destination->equals(dest2) || entry.destination->equals(dest3));
    }

    std::deque<RoutingTableEntryTupel> entriesOverC = routingTable->getAllRoutesThatLeadOver(nodeC);
    BYTES_EQUAL(1, entriesOverC.size());
    CHECK(entriesOverC.front().destination->equals(dest3));

    std::deque<RoutingTableEntryTupel> entriesOverUnkown = routingTable->getAllRoutesThatLeadOver(someUnknownNode);
    CHECK(entriesOverUnkown.empty());
}

/**
 * This test checks if an evaporation can delete elements from the routing table.
 * Also I want to know if there are any internal problems with the iterators when
 * erasing elements from the map while iterating over it (iterator invalidation).
 */
TEST(RoutingTableTest, evporateCanRemoveEntries) {
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr nodeD (new AddressMock("D"));
    AddressPtr destination (new AddressMock("dest"));

    routingTable->update(destination, nodeA, interface, 10);
    routingTable->update(destination, nodeB, interface, 0.5); // this one should be removed due to evaporation
    routingTable->update(destination, nodeC, interface, 10);
    routingTable->update(destination, nodeD, interface, 10);
    // nothing should happen right now (no time passed)
    routingTable->triggerEvaporation();

    TimeMock::letTimePass(evaporationPolicy->getTimeInterval());
    routingTable->triggerEvaporation();
    CHECK_TRUE (routingTable->exists(destination, nodeA, interface));
    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_TRUE (routingTable->exists(destination, nodeC, interface));
    CHECK_TRUE (routingTable->exists(destination, nodeD, interface));
}

TEST(RoutingTableTest, notDeliverableifOnlyRouteLeadsOverSourceNode) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));

    routingTable->update(destination, source, interface, 10);

    // start test, if the only way to a destination would lead over the source of the packet, there is effectively no route that would not introduce loops
    PacketMock packet = PacketMock("source", "destination", "someSender");
    CHECK_FALSE(routingTable->isDeliverable(&packet));
}

TEST(RoutingTableTest, doNotReturnSourceOrSenderOfAPacketAsPossibleNextHop) {
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr nextHop (new AddressMock("nextHop"));

    // prepare three routes, but only the route over nextHop is actually viable (others introduce loops)
    routingTable->update(destination, source, interface, 10);
    routingTable->update(destination, sender, interface, 10);
    routingTable->update(destination, nextHop, interface, 10);

    PacketMock packet = PacketMock("source", "destination", "sender");
    std::deque<RoutingTableEntry*> nextHops = routingTable->getPossibleNextHops(&packet);

    BYTES_EQUAL(1, nextHops.size());
    RoutingTableEntry* possibleHop = nextHops.front();
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(interface, possibleHop->getNetworkInterface());
}
