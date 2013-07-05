/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "TestMacros.h"
#include "PacketTrap.h"
#include "EnergyAwareRoutingTable.h"
#include "EARAPacketFactory.h"
#include "EARAPacket.h"
#include "TimerType.h"

#include "testAPI/mocks/EARAClientMock.h"
#include <iostream>

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;
typedef std::deque<Pair<const Packet*, AddressPtr>*> SendPacketsList;

TEST_GROUP(AbstractEARAClientTest) {
    EARAClientMock* client;
    PacketTrap* packetTrap;
    EnergyAwareRoutingTable* routingTable;
    EARAPacketFactory* packetFactory;

    void setup() {
        client = new EARAClientMock();
        packetTrap = client->getPacketTrap();
        routingTable = client->getRoutingTable();
        packetFactory = client->getPacketFactory();
    }

    void teardown() {
        delete client;
    }

    EARAPacket* castToEARAPacket(const Packet* packet) {
        EARAPacket* earaPacket = dynamic_cast<EARAPacket*>(const_cast<Packet*>(packet));
        if (earaPacket == NULL) {
            stringstream errorMessage;
            errorMessage << PacketType::getAsString(packet->getType()) << " packet with seqNr " << packet->getSequenceNumber() << " is no EARAPacket!" << endl;
            FAIL(errorMessage.str().c_str());
        }

        return earaPacket;
    }

    void createNewClient(EARAConfiguration& configuration) {
        // first delete the old client
        delete client;

        // then create a new one
        client = new EARAClientMock(configuration);
        packetTrap = client->getPacketTrap();
        routingTable = client->getRoutingTable();
        packetFactory = client->getPacketFactory();
    }
};

/**
 * In this test we let a client receive a FANT packet.
 * It is required to add its own energy value to the FANT and also to overwrite the
 * minimum energy value if necessary and then broadcast the updated FANT.
 */
TEST(AbstractEARAClientTest, aggregateEnergyInformationOfFANT) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));

    client->setEnergy(40);

    EARAPacket* fant = castToEARAPacket(packetFactory->makeFANT(source, destination, 123));
    fant->addEnergyValue(60);
    fant->decreaseTTL(4); // traveled some hops

    // sanity check
    BYTES_EQUAL(60, fant->getMinimumEnergyValue());
    BYTES_EQUAL(60, fant->getTotalEnergyValue());

    // start the test
    client->receivePacket(fant, interface);

    // we need the timer to trigger the actual sending of the FANT
    TimerMock* routeDiscoveryDelayTimer = client->getRouteDiscoveryDelayTimer(source);
    CHECK(routeDiscoveryDelayTimer != nullptr);
    CHECK(routeDiscoveryDelayTimer->getType() == TimerType::ROUTE_DISCOVERY_DELAY_TIMER);
    CHECK(routeDiscoveryDelayTimer->isRunning());
    routeDiscoveryDelayTimer->expire();

    // packet should have been updated and broadcasted
    LONGS_EQUAL(1, interface->getNumberOfSentPackets());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = interface->getSentPackets()->front();
    EARAPacket* sentPacket = castToEARAPacket(sentPacketInfo->getLeft());
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(interface->isBroadcastAddress(recipientOfSentPacket));

    BYTES_EQUAL(100, sentPacket->getTotalEnergyValue());
    BYTES_EQUAL( 40, sentPacket->getMinimumEnergyValue());
}

/**
 * In this test a client receives FANT packets which will initialize the route via
 * the sender. The client is also required to initialize the energy values for the
 * corresponding route according to the function from the master's thesis of Friedrich Große
 *
 *     Energy = Average - ( (Average-Minimum) / b )
 *
 * The constant b determines the influence of the minimum on the whole equation.
 *
 * packet came this way (energy in brackets):
 *
 * (src)--(70%)--(40%)--(70%)--(current node)
 *
 * Total:   180
 * Minimum:  40
 * Average:  60
 */
TEST(AbstractEARAClientTest, initializeEnergyValues) {
    NetworkInterfaceMock* interface;
    AddressPtr source (new AddressMock("source"));
    AddressPtr nextHop (new AddressMock("nextHop"));
    AddressPtr destination (new AddressMock("destination"));
    unsigned int seqNumber = 1;

    // start the test
    // first try with b = 2 (average and minimum are equally important)
    BasicEARAConfiguration configuration = client->getStandardConfiguration();
    configuration.setInfluenceOfMinimumEnergyValue(2);
    createNewClient(configuration);
    interface = client->createNewNetworkInterfaceMock();

    EARAPacket* fant1 = castToEARAPacket(packetFactory->makeFANT(source, destination, seqNumber++));
    fant1->setSender(nextHop);
    fant1->setMinimumEnergyValue(40);
    fant1->setTotalEnergyValue(180);
    fant1->decreaseTTL(3); // traveled 3 hops + 1 hop when the client receives the packet
    client->receivePacket(fant1, interface);
    DOUBLES_EQUAL(0.5, routingTable->getEnergyValue(source, nextHop, interface), 0.0001);

    // then with b = 3 (more weight on the average)
    configuration = client->getStandardConfiguration();
    configuration.setInfluenceOfMinimumEnergyValue(3);
    createNewClient(configuration);
    interface = client->createNewNetworkInterfaceMock();

    EARAPacket* fant2 = castToEARAPacket(packetFactory->makeFANT(source, destination, seqNumber++));
    fant2->setSender(nextHop);
    fant2->setMinimumEnergyValue(40);
    fant2->setTotalEnergyValue(180);
    fant2->decreaseTTL(3); // traveled 3 hops + 1 hop when the client receives the packet
    client->receivePacket(fant2, interface);
    DOUBLES_EQUAL(0.533333333, routingTable->getEnergyValue(source, nextHop, interface), 0.0001);

    // then with b = 5 (even more weight on the average)
    configuration = client->getStandardConfiguration();
    configuration.setInfluenceOfMinimumEnergyValue(5);
    createNewClient(configuration);
    interface = client->createNewNetworkInterfaceMock();

    EARAPacket* fant3 = castToEARAPacket(packetFactory->makeFANT(source, destination, seqNumber++));
    fant3->setSender(nextHop);
    fant3->setMinimumEnergyValue(40);
    fant3->setTotalEnergyValue(180);
    fant3->decreaseTTL(3); // traveled 3 hops + 1 hop when the client receives the packet
    client->receivePacket(fant3, interface);
    DOUBLES_EQUAL(0.56, routingTable->getEnergyValue(source, nextHop, interface), 0.0001);

    // now with b = 1 (only the minimum counts)
    configuration = client->getStandardConfiguration();
    configuration.setInfluenceOfMinimumEnergyValue(1);
    createNewClient(configuration);
    interface = client->createNewNetworkInterfaceMock();

    EARAPacket* fant4 = castToEARAPacket(packetFactory->makeFANT(source, destination, seqNumber++));
    fant4->setSender(nextHop);
    fant4->setMinimumEnergyValue(40);
    fant4->setTotalEnergyValue(180);
    fant4->decreaseTTL(3); // traveled 3 hops + 1 hop when the client receives the packet
    client->receivePacket(fant4, interface);
    DOUBLES_EQUAL(0.4, routingTable->getEnergyValue(source, nextHop, interface), 0.0001);
}

/**
 * This test checks whether a client waits for a configurable amount of time before it
 * rebroadcasts received FANTs/BANTs. This needs to be implemented so longer routes
 * have also a chance of being broadcasted if they have a higher energy fitness.
 *
 * Test setup:                | Description (Test from perspective of (X)):
 *                            |
 *   ┌────(A1)───(A)────>─┐   |   * at first (X) receives a FANT from (B) which low energy fitness
 * (..)──────(B)────────>(X)  |   * before broadcasting it any further, it waits for a very short time interval
 *   └─(C1)──(C2)──(C)──>─┘   |   * after some time another FANT from (A) and then from (C) arrives
 *                            |   * (X) is now required to broadcast only the FANT with the best energy fitness
 */
TEST(AbstractEARAClientTest, routeDiscoveryDelay) {
    BasicEARAConfiguration configuration = client->getStandardConfiguration();
    configuration.setInfluenceOfMinimumEnergyValue(3);
    configuration.setMaximumHopCount(6);
    createNewClient(configuration);

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("source"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr prevHop1 (new AddressMock("prevHop1"));
    AddressPtr prevHop2 (new AddressMock("prevHop2"));
    AddressPtr prevHop3 (new AddressMock("prevHop3"));
    AddressPtr destination (new AddressMock("destination"));
    unsigned int seqNumber = 123;

    EARAPacket* fant1 = castToEARAPacket(packetFactory->makeFANT(source, destination, seqNumber));
    fant1->decreaseTTL(1);
    fant1->setSender(nodeB);
    fant1->setPreviousHop(prevHop1);
    fant1->setMinimumEnergyValue(20);
    fant1->setTotalEnergyValue(20);
    // the path fitness of FANT 1 should evaluate to 20 * 4

    EARAPacket* fant2 = castToEARAPacket(packetFactory->makeFANT(source, destination, seqNumber));
    fant2->decreaseTTL(2);
    fant2->setSender(nodeA);
    fant2->setPreviousHop(prevHop2);
    fant2->setMinimumEnergyValue(60);
    fant2->setTotalEnergyValue(80 + 60);
    // the path fitness of FANT 1 should evaluate to 66.6666 * 3 = 200

    EARAPacket* fant3 = castToEARAPacket(packetFactory->makeFANT(source, destination, seqNumber));
    fant3->decreaseTTL(3);
    fant3->setSender(nodeC);
    fant3->setPreviousHop(prevHop3);
    fant3->setMinimumEnergyValue(70);
    fant3->setTotalEnergyValue(90 + 90 + 70);
    // the path fitness of FANT 1 should evaluate to 75.55 * 2 = 151.111

    // start test by receiving first the first FANT from (B)
    client->receivePacket(fant1, interface);
    // no packet should have been broadcasted just yet
    BYTES_EQUAL(0, sentPackets->size());
    TimerMock* routeDiscoveryDelayTimer = client->getRouteDiscoveryDelayTimer(source);
    CHECK(routeDiscoveryDelayTimer != nullptr);
    CHECK(routeDiscoveryDelayTimer->getType() == TimerType::ROUTE_DISCOVERY_DELAY_TIMER);
    CHECK(routeDiscoveryDelayTimer->isRunning());

    // now after some short time the next FANT arrives
    client->receivePacket(fant2, interface);
    //  still no packet is broadcasted (timer has not expired)
    CHECK(routeDiscoveryDelayTimer->isRunning());
    BYTES_EQUAL(0, sentPackets->size());

    // now we receive the third FANT
    client->receivePacket(fant3, interface);
    // still no packet is broadcasted
    BYTES_EQUAL(0, sentPackets->size());
    CHECK(routeDiscoveryDelayTimer->isRunning());

    // now finally the wait time is over and the client sends the best FANT (from B)
    routeDiscoveryDelayTimer->expire();

    BYTES_EQUAL(1, sentPackets->size());
    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(interface->isBroadcastAddress(receiver));

    EARAPacket* sentPacket = castToEARAPacket(sentPackets->back()->getLeft());
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(sentPacket->getSource()->equals(source));
    CHECK(sentPacket->getPreviousHop()->equals(nodeA));
    LONGS_EQUAL(seqNumber, sentPacket->getSequenceNumber());
    CHECK(sentPacket->getDestination()->equals(destination));
    LONGS_EQUAL(140 + client->getCurrentEnergyLevel(), sentPacket->getTotalEnergyValue());
    LONGS_EQUAL( 60, sentPacket->getMinimumEnergyValue());
    LONGS_EQUAL(3, sentPacket->getTTL());

    // the route discovery timer should no longer exist
    CHECK(client->getRouteDiscoveryDelayTimer(source) == nullptr);
}

/**
 * In this test we check that a new route discovery delay timer is started, if the last one
 * timed out and now there is a new route discovery
 */
TEST(AbstractEARAClientTest, routeDiscoveryDelayIsUsedForNextRouteDiscovery) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination (new AddressMock("destination"));

    EARAPacket* fant1 = castToEARAPacket(packetFactory->makeFANT(source, destination, 1));
    fant1->setSender(sender);

    // start test by receiving first the first FANT
    client->receivePacket(fant1, interface);

    // no packet should have been broadcasted just yet
    BYTES_EQUAL(0, sentPackets->size());
    TimerMock* routeDiscoveryDelayTimer = client->getRouteDiscoveryDelayTimer(source);
    CHECK(routeDiscoveryDelayTimer != nullptr);
    CHECK(routeDiscoveryDelayTimer->getType() == TimerType::ROUTE_DISCOVERY_DELAY_TIMER);
    CHECK(routeDiscoveryDelayTimer->isRunning());

    // now the first timer expires and the FANT should be broadcasted again
    routeDiscoveryDelayTimer->expire();
    BYTES_EQUAL(1, sentPackets->size());
    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(interface->isBroadcastAddress(receiver));

    EARAPacket* sentPacket = castToEARAPacket(sentPackets->back()->getLeft());
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(sentPacket->getSource()->equals(source));
    LONGS_EQUAL(1, sentPacket->getSequenceNumber());
    CHECK(sentPacket->getDestination()->equals(destination));

    // the route discovery timer should no longer exist
    CHECK(client->getRouteDiscoveryDelayTimer(source) == nullptr);

    // now we start a new route discovery
    EARAPacket* fant2 = castToEARAPacket(packetFactory->makeFANT(source, destination, 2));
    fant2->setSender(sender);
    client->receivePacket(fant2, interface);

    // again the client is required to hold the packet back via the delay timer
    BYTES_EQUAL(1, sentPackets->size());
    routeDiscoveryDelayTimer = client->getRouteDiscoveryDelayTimer(source);
    CHECK(routeDiscoveryDelayTimer != nullptr);
    CHECK(routeDiscoveryDelayTimer->getType() == TimerType::ROUTE_DISCOVERY_DELAY_TIMER);
    CHECK(routeDiscoveryDelayTimer->isRunning());

    // now the second timer also expires and the FANT should be broadcasted again
    routeDiscoveryDelayTimer->expire();
    BYTES_EQUAL(2, sentPackets->size());
    receiver = sentPackets->back()->getRight();
    CHECK(interface->isBroadcastAddress(receiver));

    sentPacket = castToEARAPacket(sentPackets->back()->getLeft());
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(sentPacket->getSource()->equals(source));
    LONGS_EQUAL(2, sentPacket->getSequenceNumber());
    CHECK(sentPacket->getDestination()->equals(destination));
}

/**
 * In this test we have two route discoveries.
 * The first one will initialize the energy values and the
 * second one will refresh them with the current values.
 */
TEST(AbstractEARAClientTest, newRouteDiscoveryRefreshesEnergyValues) {
    BasicEARAConfiguration configuration = client->getStandardConfiguration();
    configuration.setInfluenceOfMinimumEnergyValue(3);
    configuration.setMaximumHopCount(6);
    createNewClient(configuration);

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr source (new AddressMock("source"));
    AddressPtr nextHop (new AddressMock("nextHop"));
    AddressPtr destination (new AddressMock("destination"));

    EARAPacket* fant1 = castToEARAPacket(packetFactory->makeFANT(source, destination, 1));
    fant1->setSender(nextHop);
    fant1->decreaseTTL(3); // + receiving that makes a total of 4 hops
    fant1->setTotalEnergyValue(120); // average will be 40
    fant1->setMinimumEnergyValue(10);

    client->receivePacket(fant1, interface);
    DOUBLES_EQUAL(0.3, routingTable->getEnergyValue(source, nextHop, interface), 0.0001);

    // after some time there is a new route discovery (maybe for another destination)
    AddressPtr destination2 (new AddressMock("destination2"));
    EARAPacket* fant2 = castToEARAPacket(packetFactory->makeFANT(source, destination2, 2));
    fant2->setSender(nextHop);
    fant2->decreaseTTL(3); // + receiving that makes a total of 4 hops
    fant2->setTotalEnergyValue(60); // average will now be only 20
    fant2->setMinimumEnergyValue(20);

    //the energy should be updated even though no new route is created
    client->receivePacket(fant2, interface);
    DOUBLES_EQUAL(0.2, routingTable->getEnergyValue(source, nextHop, interface), 0.0001);
}

/**
 * We check that FANTs and BANTs which arrive at their respective destination are not broadcasted any more
 * but are evaluated like in the regular ARA.
 */
TEST(AbstractEARAClientTest, fantsAndBantsArestillProcessedByTheirDestinations) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    unsigned int lastSequenceNumber = client->getNextSequenceNumber();

    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination = interface->getLocalAddress();
    Packet* fant = packetFactory->makeFANT(source, destination, 123);

    // start the test
    client->receivePacket(fant, interface);

    // check the sent packet
    CHECK(sentPackets->size() == 1);
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(interface->isBroadcastAddress(sentPacketInfo->getRight()));
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK_EQUAL(PacketType::BANT, sentPacket->getType());
    CHECK(sentPacket->getSource()->equals(destination));
    CHECK(sentPacket->getDestination()->equals(source));
    CHECK(sentPacket->getSender()->equals(destination));
    CHECK(sentPacket->getPreviousHop()->equals(destination));
    LONGS_EQUAL(lastSequenceNumber+1, sentPacket->getSequenceNumber());
}

/**
 * In this test we check that the destination starts to send PEANTS through the network
 * if their energy level has changed enough.
 */
TEST(AbstractEARAClientTest, destinationsStartToSendPEANTsIfEnergyHasChangedNoticeably) {
    // the client should start to send out PEANTs if its own energy has decreased at least 1 % percent since the first traffic has arrived or the last PEANT has been send
    BasicEARAConfiguration configuration = client->getStandardConfiguration();
    configuration.setPEANTEnergyThreshold(0.01);
    createNewClient(configuration);
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination = interface->getLocalAddress();

    // set the initial energy level
    client->setEnergy(100);
    //TODO set the lastPEANTEnergyLevel to this value in the mock!

    // start the test by receiving the first traffic to this destination
    Packet* packet1 = packetFactory->makeDataPacket(source, destination, 123, "Hello World", 12);
    client->receivePacket(packet1, interface);
    // check that no PEANT or any other packet has been sent by this client
    CHECK(sentPackets->size() == 0);

    // update energy level to 1% less than the initial level
    client->setEnergy(99);

    // receive another packet to this destination
    Packet* packet2 = packetFactory->makeDataPacket(source, destination, 124, "Foo", 4);
    client->receivePacket(packet2, interface);

    CHECK(sentPackets->size() == 1);
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(interface->isBroadcastAddress(sentPacketInfo->getRight()));
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK_EQUAL(PacketType::PEANT, sentPacket->getType());
    CHECK(sentPacket->getSource()->equals(destination));
    CHECK(sentPacket->getSender()->equals(destination));
    CHECK(sentPacket->getPreviousHop()->equals(destination));

    // there is no real destination in a PEANT so it doesn't matter what address we choose here
    // however, we need to have something in the address field so I chose the source of the PEANt
    // itself so nobody will ever be tempted to process this packet like it was meant only to him
    CHECK(sentPacket->getDestination()->equals(destination));
}

//TODO test that PEANTs feature is disabled if the energyThreshold is -1
