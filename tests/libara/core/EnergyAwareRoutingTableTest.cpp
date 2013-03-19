/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "EnergyAwareRoutingTable.h"
#include "LinearEvaporationPolicy.h"
#include "EnergyAwareRoutingTableEntry.h"
#include "PacketType.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/time/TimeMock.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(EnergyAwareRoutingTableTest) {
    EnergyAwareRoutingTable* routingTable;
    LinearEvaporationPolicyMock* evaporationPolicy;

    void setup() {
        evaporationPolicy = new LinearEvaporationPolicyMock();
        routingTable = new EnergyAwareRoutingTable();
        routingTable->setEvaporationPolicy(evaporationPolicy);
    }

    void teardown() {
        delete routingTable;
        delete evaporationPolicy;
    }
};

TEST(EnergyAwareRoutingTableTest, getPossibleNextHopsReturnsEmptyList) {
    AddressPtr destination (new AddressMock());

    std::deque<RoutingTableEntry*>* list = routingTable->getPossibleNextHops(destination);
    CHECK(list->empty());

    delete list;
}

TEST(EnergyAwareRoutingTableTest, unregisteredAddressIsNotDeliverable) {
    AddressPtr destinationAddress (new AddressMock());

    CHECK(routingTable->isDeliverable(destinationAddress) == false);
}

TEST(EnergyAwareRoutingTableTest, packetWithUnregisteredAddressIsNotDeliverable) {
    PacketMock packet = PacketMock();

    CHECK(routingTable->isDeliverable(&packet) == false);
}

TEST(EnergyAwareRoutingTableTest, updateEnergyAwareRoutingTable) {
    PacketMock packet = PacketMock();
    AddressPtr destination = packet.getDestination();
    AddressPtr nextHop (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    float pheromoneValue = 123.456;
    float energyValue = 789.123;

    CHECK(routingTable->isDeliverable(&packet) == false);
    routingTable->update(destination, nextHop, &interface, pheromoneValue, energyValue);

    CHECK(routingTable->isDeliverable(&packet));
    std::deque<RoutingTableEntry*>* nextHops = routingTable->getPossibleNextHops(&packet);
    CHECK(nextHops->size() == 1);
    EnergyAwareRoutingTableEntry* possibleHop = static_cast<EnergyAwareRoutingTableEntry*>(nextHops->front());
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());
    CHECK_EQUAL(energyValue, possibleHop->getEnergyValue());
}

TEST(EnergyAwareRoutingTableTest, overwriteExistingEntryWithUpdate) {
    PacketMock packet = PacketMock();
    AddressPtr destination = packet.getDestination();
    AddressPtr nextHop (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    float pheromoneValue = 123.456;
    float energyValue = 789.123;

    // first we register a route to a destination the first time
    routingTable->update(destination, nextHop, &interface, pheromoneValue, energyValue);

    CHECK(routingTable->isDeliverable(&packet));
    std::deque<RoutingTableEntry*>* nextHops = routingTable->getPossibleNextHops(&packet);
    BYTES_EQUAL(1, nextHops->size());
    EnergyAwareRoutingTableEntry* possibleHop = static_cast<EnergyAwareRoutingTableEntry*>(nextHops->front());
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(pheromoneValue, possibleHop->getPheromoneValue());
    CHECK_EQUAL(energyValue, possibleHop->getEnergyValue());

    // now we want to update the pheromone value of this route
    routingTable->update(destination, nextHop, &interface, 42, 23);
    nextHops = routingTable->getPossibleNextHops(&packet);
    BYTES_EQUAL(1, nextHops->size());
    possibleHop = static_cast<EnergyAwareRoutingTableEntry*>(nextHops->front());
    CHECK(nextHop->equals(possibleHop->getAddress()));
    CHECK_EQUAL(&interface, possibleHop->getNetworkInterface());
    CHECK_EQUAL(42, possibleHop->getPheromoneValue());
    CHECK_EQUAL(23, possibleHop->getEnergyValue());
}

TEST(EnergyAwareRoutingTableTest, getPossibleNextHops) {
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination1 (new AddressMock("Destination1"));
    AddressPtr destination2 (new AddressMock("Destination2"));

    AddressPtr nextHop1a (new AddressMock("nextHop1a"));
    AddressPtr nextHop1b (new AddressMock("nextHop1b"));
    AddressPtr nextHop2 (new AddressMock("nextHop2"));
    AddressPtr nextHop3 (new AddressMock("nextHop3"));
    AddressPtr nextHop4 (new AddressMock("nextHop4"));
    NetworkInterfaceMock interface1 = NetworkInterfaceMock();
    NetworkInterfaceMock interface2 = NetworkInterfaceMock();
    NetworkInterfaceMock interface3 = NetworkInterfaceMock();

    float pheromoneValue1a = 1;
    float pheromoneValue1b = 5;
    float pheromoneValue2 = 2.3;
    float pheromoneValue3 = 4;
    float pheromoneValue4 = 2;

    float energyValue1a = 1;
    float energyValue1b = 5;
    float energyValue2 = 2.3;
    float energyValue3 = 4;
    float energyValue4 = 2;

    routingTable->update(destination1, nextHop1a, &interface1, pheromoneValue1a, energyValue1a);
    routingTable->update(destination1, nextHop1b, &interface1, pheromoneValue1b, energyValue1b);
    routingTable->update(destination1, nextHop2, &interface2, pheromoneValue2, energyValue2);

    routingTable->update(destination2, nextHop3, &interface3, pheromoneValue3, energyValue3);
    routingTable->update(destination2, nextHop4, &interface1, pheromoneValue4, energyValue4);

    std::deque<RoutingTableEntry*>* nextHopsForDestination1 = routingTable->getPossibleNextHops(destination1);
    BYTES_EQUAL(3, nextHopsForDestination1->size());
    for (unsigned int i = 0; i < nextHopsForDestination1->size(); i++) {
        EnergyAwareRoutingTableEntry* possibleHop = static_cast<EnergyAwareRoutingTableEntry*>(nextHopsForDestination1->at(i));
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop1a)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1a, possibleHop->getPheromoneValue());
            CHECK_EQUAL(energyValue1a, possibleHop->getEnergyValue());
        }
        else if(hopAddress->equals(nextHop1b)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue1b, possibleHop->getPheromoneValue());
            CHECK_EQUAL(energyValue1b, possibleHop->getEnergyValue());
        }
        else if(hopAddress->equals(nextHop2)) {
            CHECK_EQUAL(&interface2, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue2, possibleHop->getPheromoneValue());
            CHECK_EQUAL(energyValue2, possibleHop->getEnergyValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop1a, nextHop1b or nextHop2
        }
    }

    std::deque<RoutingTableEntry*>* nextHopsForDestination2 = routingTable->getPossibleNextHops(destination2);
    BYTES_EQUAL(2, nextHopsForDestination2->size());
    for (unsigned int i = 0; i < nextHopsForDestination2->size(); i++) {
        EnergyAwareRoutingTableEntry* possibleHop = static_cast<EnergyAwareRoutingTableEntry*>(nextHopsForDestination2->at(i));
        AddressPtr hopAddress = possibleHop->getAddress();
        if(hopAddress->equals(nextHop3)) {
            CHECK_EQUAL(&interface3, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue3, possibleHop->getPheromoneValue());
            CHECK_EQUAL(energyValue3, possibleHop->getEnergyValue());
        }
        else if(hopAddress->equals(nextHop4)) {
            CHECK_EQUAL(&interface1, possibleHop->getNetworkInterface());
            CHECK_EQUAL(pheromoneValue4, possibleHop->getPheromoneValue());
            CHECK_EQUAL(energyValue4, possibleHop->getEnergyValue());
        }
        else {
            CHECK(false); // hops for this destination must either be nextHop3 or nextHop4
        }
    }
}

TEST(EnergyAwareRoutingTableTest, getPheromoneValue) {
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nextHopAddress (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // Should be zero because there is no known route to this destination
    LONGS_EQUAL(0, routingTable->getPheromoneValue(destination, nextHopAddress, &interface));

    routingTable->update(destination, nextHopAddress, &interface, 123, 234);
    LONGS_EQUAL(123, routingTable->getPheromoneValue(destination, nextHopAddress, &interface));
}

TEST(EnergyAwareRoutingTableTest, getEnergyValue) {
    AddressPtr sourceAddress (new AddressMock("Source"));
    AddressPtr destination (new AddressMock("Destination"));
    AddressPtr nextHopAddress (new AddressMock("nextHop"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    // Should be zero because there is no known route to this destination
    LONGS_EQUAL(0, routingTable->getPheromoneValue(destination, nextHopAddress, &interface));

    routingTable->update(destination, nextHopAddress, &interface, 123, 234);
    LONGS_EQUAL(234, routingTable->getEnergyValue(destination, nextHopAddress, &interface));
}

TEST(EnergyAwareRoutingTableTest, removeEntry) {
    AddressPtr destination (new AddressMock("Destination"));

    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));

    NetworkInterfaceMock interface = NetworkInterfaceMock();

    routingTable->update(destination, nodeA, &interface, 2.5, 3.6);
    routingTable->update(destination, nodeB, &interface, 3.7, 5.8);
    routingTable->update(destination, nodeC, &interface, 1.2, 10.0);

    // start the test
    routingTable->removeEntry(destination, nodeB, &interface);
    std::deque<RoutingTableEntry*>* possibleNextHops = routingTable->getPossibleNextHops(destination);
    for(auto& entry: *possibleNextHops) {
        if(entry->getAddress()->equals(nodeB)) {
            FAIL("The deleted hop should not longer be in the list of possible next hops");
        }
    }
}

TEST(EnergyAwareRoutingTableTest, evaporatePheromones) {
    AddressPtr destination (new AddressMock("Destination"));
    NetworkInterfaceMock interface = NetworkInterfaceMock();

    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    float pheromoneValueA = 2.5f;
    float energyValueA = 2.5f;
    float pheromoneValueB = 3.8f;
    float energyValueB = 3.8f;
    float pheromoneValueC = 0.2f;
    float energyValueC = 0.2f;

    routingTable->update(destination, nodeA, &interface, pheromoneValueA, energyValueA);
    routingTable->update(destination, nodeB, &interface, pheromoneValueB, energyValueB);
    routingTable->update(destination, nodeC, &interface, pheromoneValueC, energyValueC);

    // no time has passed so nothing is evaporated
    CHECK_EQUAL(pheromoneValueA, routingTable->getPheromoneValue(destination, nodeA, &interface));
    CHECK_EQUAL(pheromoneValueB, routingTable->getPheromoneValue(destination, nodeB, &interface));
    CHECK_EQUAL(pheromoneValueC, routingTable->getPheromoneValue(destination, nodeC, &interface));

    // let some time pass to trigger the evaporation
    TimeMock::letTimePass(evaporationPolicy->getTimeInterval());
    float evaporationFactor = evaporationPolicy->getEvaporationFactor();
    DOUBLES_EQUAL(pheromoneValueA * evaporationFactor, routingTable->getPheromoneValue(destination, nodeA, &interface), 0.00001);
    DOUBLES_EQUAL(pheromoneValueB * evaporationFactor, routingTable->getPheromoneValue(destination, nodeB, &interface), 0.00001);

    // pheromoneValueC should be well below the threshold and therefore be zero
    CHECK_EQUAL(0.0, routingTable->getPheromoneValue(destination, nodeC, &interface));
}
