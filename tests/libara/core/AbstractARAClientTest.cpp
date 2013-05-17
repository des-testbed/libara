/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "TestMacros.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "PacketType.h"
#include "Address.h"
#include "Pair.h"
#include "Logger.h"
#include "Environment.h"

#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/RoutingTableMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/LoggerMock.h"
#include "testAPI/mocks/time/ClockMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;
typedef std::deque<Pair<const Packet*, AddressPtr>*> SendPacketsList;

TEST_GROUP(AbstractARAClientTest) {
    ARAClientMock* client;
    PacketTrap* packetTrap;
    RoutingTableMock* routingTable;
    PacketFactory* packetFactory;

    void setup() {
        client = new ARAClientMock();
        packetTrap = client->getPacketTrap();
        routingTable = (RoutingTableMock*) client->getRoutingTable();
        packetFactory = client->getPacketFactory();
    }

    void teardown() {
        delete client;
    }

    /**
     * Returns true iff a route to destination via nextHop and interface is known in the RoutingTable.
     */
    bool routeIsKnown(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
        return routingTable->exists(destination, nextHop, interface);
    }
};

TEST(AbstractARAClientTest, packetGetsTrappedIfNotDeliverable) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    PacketMock* packet = new PacketMock();
    packet->setSource(interface->getLocalAddress());

    CHECK(routingTable->isDeliverable(packet) == false);
    client->sendPacket(packet);
    CHECK(packetTrap->contains(packet));
}

TEST(AbstractARAClientTest, generalBroadCast) {
    NetworkInterfaceMock* interface1 = client->createNewNetworkInterfaceMock();
    NetworkInterfaceMock* interface2 = client->createNewNetworkInterfaceMock();
    NetworkInterfaceMock* interface3 = client->createNewNetworkInterfaceMock();

    Packet* packet = new PacketMock();
    Packet* clone = packetFactory->makeClone(packet); // we need to create this clone because the original packet will be deleted directly in the broadcast method
    client->broadCast(packet);

    CHECK(interface1->hasPacketBeenBroadCasted(clone) == true);
    CHECK(interface2->hasPacketBeenBroadCasted(clone) == true);
    CHECK(interface3->hasPacketBeenBroadCasted(clone) == true);

    delete clone;
}

TEST(AbstractARAClientTest, getNextSequenceNumber) {
    for (unsigned int i = 1; i < 100; i++) {
        CHECK_EQUAL(i, client->getNextSequenceNumber());
    }
}

TEST(AbstractARAClientTest, broadcastFANTIfPacketNotDeliverable) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    Packet* packet = new Packet(source, destination, source, PacketType::DATA, 123, 10);

    CHECK(routingTable->isDeliverable(packet) == false);
    client->sendPacket(packet);

    LONGS_EQUAL(1, interface->getNumberOfSentPackets());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = interface->getSentPackets()->front();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(interface->isBroadcastAddress(recipientOfSentPacket));
}

TEST(AbstractARAClientTest, sendPacketToNextHopIfRouteIsKnown) {
    NetworkInterfaceMock* interface1 = client->createNewNetworkInterfaceMock();
    NetworkInterfaceMock* interface2 = client->createNewNetworkInterfaceMock();
    NetworkInterfaceMock* interface3 = client->createNewNetworkInterfaceMock();
    AddressPtr nextHop (new AddressMock("nextHop"));

    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    unsigned int originalTTL = 5;
    unsigned int originalSequenceNr = 123;
    const char* originalPayload = "Hello";
    int originalPayloadSize = std::strlen(originalPayload) + 1;
    Packet* packet = new Packet(originalSource, originalDestination, originalSender, PacketType::DATA, originalSequenceNr, originalTTL, originalPayload, originalPayloadSize);

    // make sure that a route to the packet destination is available
    routingTable->update(packet->getDestination(), nextHop, interface2, 1.0);
    CHECK(routingTable->isDeliverable(packet) == true);

    client->sendPacket(packet);

    // check if packet has been send over the correct interface
    CHECK_EQUAL(0, interface1->getNumberOfSentPackets());
    CHECK_EQUAL(1, interface2->getNumberOfSentPackets());
    CHECK_EQUAL(0, interface3->getNumberOfSentPackets());

    // check if packet has been send via interface2 to nextHop
    Pair<const Packet*, AddressPtr>* sentPacketInfo = interface2->getSentPackets()->front();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();
    CHECK(recipientOfSentPacket->equals(nextHop));

    // Check that packet content is basically the same
    CHECK_EQUAL(PacketType::DATA, sentPacket->getType());
    CHECK(originalSource->equals(sentPacket->getSource()));
    CHECK(originalDestination->equals(sentPacket->getDestination()));
    CHECK_EQUAL(originalSequenceNr, sentPacket->getSequenceNumber());
    STRCMP_EQUAL(originalPayload, sentPacket->getPayload());
    LONGS_EQUAL(originalPayloadSize, sentPacket->getPayloadLength());

    // the TTL would be decremented on receive of the next hop
    CHECK_EQUAL(originalTTL, sentPacket->getTTL());
}

TEST(AbstractARAClientTest, getNumberOfNetworkInterfaces) {
    NetworkInterfaceMock interface1 = NetworkInterfaceMock();
    NetworkInterfaceMock interface2 = NetworkInterfaceMock();
    NetworkInterfaceMock interface3 = NetworkInterfaceMock();
    CHECK_EQUAL(0, client->getNumberOfNetworkInterfaces());

    client->addNetworkInterface(&interface1);
    CHECK_EQUAL(1, client->getNumberOfNetworkInterfaces());

    client->addNetworkInterface(&interface2);
    CHECK_EQUAL(2, client->getNumberOfNetworkInterfaces());

    client->addNetworkInterface(&interface3);
    CHECK_EQUAL(3, client->getNumberOfNetworkInterfaces());
}

TEST(AbstractARAClientTest, getNetworkInterface) {
    NetworkInterfaceMock interface1 = NetworkInterfaceMock();
    NetworkInterfaceMock interface2 = NetworkInterfaceMock();
    NetworkInterfaceMock interface3 = NetworkInterfaceMock();
    client->addNetworkInterface(&interface1);
    client->addNetworkInterface(&interface2);
    client->addNetworkInterface(&interface3);

    CHECK(client->getNetworkInterface(0) == &interface1);
    CHECK(client->getNetworkInterface(1) == &interface2);
    CHECK(client->getNetworkInterface(2) == &interface3);
}

TEST(AbstractARAClientTest, registerReceivedPacket) {
    PacketMock packet1 = PacketMock("A", "B", 123);
    PacketMock packet2 = PacketMock("A", "B", 124);  // same source but different seqNr
    PacketMock packet3 = PacketMock("C", "A", 123);  // same seqNr but different source
    PacketMock packet4 = PacketMock("A", "C", 123);  // the source and seqNr are equal to packet1

    CHECK_FALSE(client->hasBeenReceivedEarlier(&packet1));
    CHECK_FALSE(client->hasBeenReceivedEarlier(&packet2));
    CHECK_FALSE(client->hasBeenReceivedEarlier(&packet3));
    CHECK_FALSE(client->hasBeenReceivedEarlier(&packet4));

    client->registerReceivedPacket(&packet1);
    CHECK_TRUE(client->hasBeenReceivedEarlier(&packet1));
    CHECK_FALSE(client->hasBeenReceivedEarlier(&packet2));
    CHECK_FALSE(client->hasBeenReceivedEarlier(&packet3));
    CHECK_TRUE(client->hasBeenReceivedEarlier(&packet4));
}

/**
 * In this test we simulate that a loop occurred in the routing:
 * At first A receives a packet from node src and relays it to B
 * Than it receives the same packet from C which is where the loop
 * must be detected
 * Node A must respond to node C with a DUPLICATE_ERROR packet.
 *
 * Test topology:
 *
 * (src)--->(A)--->(B)--->(C)--->(dest)
 *           └-----------<-┘
 */
TEST(AbstractARAClientTest, loopDetection) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    AddressPtr source (new AddressMock("src"));
    AddressPtr destination (new AddressMock("dest"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    Packet* packet1 = new Packet(source, destination, source, PacketType::DATA, 123, 10);
    Packet* packet2 = new Packet(source, destination, nodeC, PacketType::DATA, 123, 7);
    int maxNrOfHops = packetFactory->getMaximumNrOfHops();
    routingTable->update(destination, nodeB, interface, 10.0);

    // start the test
    client->receivePacket(packet1, interface);
    // the client should have relayed the first packet
    LONGS_EQUAL(1, interface->getNumberOfSentPackets());

    client->receivePacket(packet2, interface);
    // the client should have sent a duplicate warning to C
    LONGS_EQUAL(2, interface->getNumberOfSentPackets());

    // check the contents of the duplicate warning packet
    Pair<const Packet*, AddressPtr>* duplicateWarningPacketInfo = interface->getSentPackets()->at(1);
    const Packet* duplicateWarning = duplicateWarningPacketInfo->getLeft();
    AddressPtr recipientOfDuplicateWarning = duplicateWarningPacketInfo->getRight();
    CHECK(recipientOfDuplicateWarning->equals(nodeC));
    CHECK(duplicateWarning->getSender()->equals(interface->getLocalAddress()));
    CHECK(duplicateWarning->getSource()->equals(interface->getLocalAddress()));
    CHECK(duplicateWarning->getDestination()->equals(destination));
    CHECK(duplicateWarning->getType() == PacketType::DUPLICATE_ERROR);
    LONGS_EQUAL(maxNrOfHops, duplicateWarning->getTTL());
    CHECK_EQUAL(0, duplicateWarning->getPayloadLength());
}

/**
 * In this test we simulate that a FANT or a BANT have been received
 * twice at node x. This kind of duplication must simply be ignored
 * by node x (no further broadcasting of the ant packet).
 */
TEST(AbstractARAClientTest, ignoreDuplicateAntPackets) {
    // prepare the ant packets
    Packet* fant1 = new PacketMock("A", "B", "C", 123, 3, PacketType::FANT);
    Packet* fant2 = new PacketMock("A", "B", "C", 123, 3, PacketType::FANT);
    Packet* bant1 = new PacketMock("B", "A", "C", 456, 4, PacketType::BANT);
    Packet* bant2 = new PacketMock("B", "A", "C", 456, 4, PacketType::BANT);
    Packet* bant3 = new PacketMock("B", "A", "C", 456, 4, PacketType::BANT);

    // let client receive the packets over the same interface twice or more
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("X");
    client->receivePacket(fant1, interface);
    client->receivePacket(fant2, interface);
    client->receivePacket(bant1, interface);
    client->receivePacket(bant2, interface);
    client->receivePacket(bant3, interface);

    LONGS_EQUAL(2, interface->getNumberOfSentPackets()); // only two broadcasts (1 FANT & 1 BANT) and nothing more
    SendPacketsList* sendPackets = interface->getSentPackets();
    CHECK_EQUAL(PacketType::FANT, sendPackets->at(0)->getLeft()->getType());
    CHECK_EQUAL(PacketType::BANT, sendPackets->at(1)->getLeft()->getType());
}

/**
 * This test checks whether the routing table is updated if a new packet has been received.
 * The packet will be send from node A to node B via node C and the current node D.
 * After the packet has been processed at node D a route to node A (back via node C) must be
 * known to node D.
 *
 * (A)-->(C)-->(D)-->(B)
 *
 * Note that we do not check how the table is updated at this point because this depends
 * on the concrete implementation of the AbstractARAclient->
 */
TEST(AbstractARAClientTest, routingTableIsUpdated) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock();
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    Packet* packet = new Packet(nodeA, nodeB, nodeC, PacketType::DATA, 123, 10);

    CHECK(routingTable->isDeliverable(nodeA) == false);
    client->receivePacket(packet, interface);
    CHECK(routingTable->isDeliverable(nodeA) == true);
    std::deque<RoutingTableEntry*> possibleHops = routingTable->getPossibleNextHops(nodeA);
    LONGS_EQUAL(1, possibleHops.size());
    CHECK(possibleHops.front()->getAddress()->equals(nodeC));
    CHECK(possibleHops.front()->getNetworkInterface()->equals(interface));
}

/**
 * This test checks if a data packet that is received at the current node
 * (with address 192.168.0.1) is delivered to the system if it is directed
 * to the current node.
 */
TEST(AbstractARAClientTest, dataPacketIsDeliveredToSystem) {
    std::deque<const Packet*>* deliveredPackets = client->getDeliveredPackets();
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("192.168.0.1");
    Packet* packetToThisNode = new PacketMock("192.168.0.4", "192.168.0.1",   124);  // directed to this node
    Packet* anotherPacket = new PacketMock("192.168.0.4", "192.168.0.123", 123, 1, PacketType::DATA);// directed to some other node

    CHECK(deliveredPackets->empty())

    client->receivePacket(anotherPacket, interface);
    CHECK(deliveredPackets->empty())

    client->receivePacket(packetToThisNode, interface);
    LONGS_EQUAL(1, deliveredPackets->size());
    CHECK(deliveredPackets->front()->equals(packetToThisNode));

    delete packetToThisNode;
}

/**
 * In this test a node A receives a data packet from node B which is
 * directed to another node C. The route to that node is known.
 * It is expected that the packet is relayed through the known route.
 */
TEST(AbstractARAClientTest, dataPacketIsRelayedIfRouteIsKnown) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();

    AddressPtr source (new AddressMock("B"));
    AddressPtr destination (new AddressMock("C"));
    AddressPtr sender = source;
    int ttl = 10;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, 123, ttl, "Hello World");

    // create a route to the destination
    routingTable->update(destination, destination, interface, 10);

    // start the test
    client->receivePacket(packet, interface);
    CHECK(sentPackets->size() == 1);
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(sentPacketInfo->getRight()->equals(destination)); // packet has been sent to destination

    // check the sent packet
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getSource()->equals(source));
    CHECK(sentPacket->getDestination()->equals(destination));
    CHECK(sentPacket->getSender()->equals(interface->getLocalAddress()));
    CHECK_EQUAL(PacketType::DATA, sentPacket->getType());
    STRCMP_EQUAL("Hello World", sentPacket->getPayload());
    LONGS_EQUAL(123, sentPacket->getSequenceNumber());
    LONGS_EQUAL(ttl-1, sentPacket->getTTL());
}

/**
 * In this test node A receives a FANT from node B which is directed to
 * node C. The FANT is expected to be broadcast over all interfaces.
 * Node A does also receive a BANT back from C directed to node B which
 * is also expected to be broadcast.
 */
TEST(AbstractARAClientTest, receivedAntPacketsAreBroadcasted) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();

    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    int maxHopCount = 10;
    Packet* fantPacket = new Packet(nodeB, nodeC, nodeB, PacketType::FANT, 123, maxHopCount);
    Packet* bantPacket = new Packet(nodeC, nodeB, nodeC, PacketType::BANT, 345, maxHopCount);

    // start the test
    client->receivePacket(fantPacket, interface);
    client->receivePacket(bantPacket, interface);

    CHECK(sentPackets->size() == 2);
    Pair<const Packet*, AddressPtr>* sentPacketInfo1 = sentPackets->at(0);
    Pair<const Packet*, AddressPtr>* sentPacketInfo2 = sentPackets->at(1);

    // check the first sent packet
    CHECK(interface->isBroadcastAddress(sentPacketInfo1->getRight()));
    const Packet* sentPacket1 = sentPacketInfo1->getLeft();
    CHECK(sentPacket1->getSource()->equals(nodeB));
    CHECK(sentPacket1->getDestination()->equals(nodeC));
    CHECK(sentPacket1->getSender()->equals(interface->getLocalAddress()));
    CHECK_EQUAL(PacketType::FANT, sentPacket1->getType());
    LONGS_EQUAL(maxHopCount-1, sentPacket1->getTTL());

    // check the first sent packet
    const Packet* sentPacket2 = sentPacketInfo2->getLeft();
    CHECK(sentPacket2->getSource()->equals(nodeC));
    CHECK(sentPacket2->getDestination()->equals(nodeB));
    CHECK(sentPacket2->getSender()->equals(interface->getLocalAddress()));
    CHECK_EQUAL(PacketType::BANT, sentPacket2->getType());
    LONGS_EQUAL(maxHopCount-1, sentPacket2->getTTL());
}

/**
 * In this test node A receives a FANT directed from node B via node C
 * to node A itself. It is expected to broadcast a new BANT directed to
 * the FANts source.
 *
 * (B)--FANT->(C)--FANT->(A)
 *  └-<-BANT--(C)<-BANT---┘
 */
TEST(AbstractARAClientTest, receivedFANTTriggersNewBANT) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    unsigned int lastSequenceNumber = client->getNextSequenceNumber();
    int maxHopCount = 15;
    client->setMaxHopCount(maxHopCount);

    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeC (new AddressMock("C"));
    Packet* fant = new Packet(nodeB, nodeA, nodeC, PacketType::FANT, 123, maxHopCount);

    // start the test
    client->receivePacket(fant, interface);

    // check the sent packet
    CHECK(sentPackets->size() == 1);
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(interface->isBroadcastAddress(sentPacketInfo->getRight()));
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getSource()->equals(nodeA));
    CHECK(sentPacket->getDestination()->equals(nodeB));
    CHECK(sentPacket->getSender()->equals(nodeA));
    CHECK(sentPacket->getPreviousHop()->equals(nodeA));
    CHECK_EQUAL(PacketType::BANT, sentPacket->getType());
    LONGS_EQUAL(maxHopCount, sentPacket->getTTL());
    LONGS_EQUAL(lastSequenceNumber+1, sentPacket->getSequenceNumber());
}

/**
 * In this test we check if a client broadcasts a FANT again that he has
 * initially created and sent in the first place.
 */
TEST(AbstractARAClientTest, doNotReBroadcastFANT) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    Packet* data = new PacketMock("A", "Z", "A", 123, 10, PacketType::DATA);

    // sending the initial packet should trigger a FANT broadcast
    client->sendPacket(data);
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->at(0);
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getType() == PacketType::FANT);

    // cleanup
    sentPackets->clear();
    delete sentPacketInfo;

    // emulate that the neighbor does also broadcast the FANT and this client receives it
    AddressPtr neighborAddress (new AddressMock("B"));
    Packet* answer = packetFactory->makeClone(sentPacket);
    answer->decreaseTTL();
    answer->setSender(neighborAddress);
    client->receivePacket(answer, interface);

    // the client should not broadcast the FANT again
    CHECK(sentPackets->empty())

    // delete the sent packet (destructor does not know about it because we cleared the list of sent packets earlier)
    delete sentPacket;
}

TEST(AbstractARAClientTest, doNotReBroadcastBANT) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("Z");
    SendPacketsList* sentPackets = interface->getSentPackets();
    Packet* fant = new PacketMock("A", "Z", "A", 123, 10, PacketType::FANT);

    // client receives the FANT that is directed to him (should trigger BANt broadcast)
    client->receivePacket(fant, interface);
    CHECK(sentPackets->size() == 1);
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->at(0);
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getType() == PacketType::BANT);

    // cleanup
    sentPackets->clear();
    delete sentPacketInfo;

    // emulate that the neighbor does also broadcast the BANT and this client receives it back
    AddressPtr neighborAddress (new AddressMock("Y"));
    Packet* answer = packetFactory->makeClone(sentPacket);
    answer->decreaseTTL();
    answer->setSender(neighborAddress);
    client->receivePacket(answer, interface);

    // the client should not broadcast the BANT again
    CHECK(sentPackets->empty())

    // delete the sent packet (destructor does not know about it because we cleared the list of sent packets earlier)
    delete sentPacket;
}

/**
 * In this test we check if a client deletes the routing table entry to
 * another note if he receives a duplicate (loop) warning from this node.
 *
 * Test setup:
 *  - we test at node B
 *  - B receives the duplicate warning from A and is required to delete
 *    the link to A for the destination givenin the packet
 *
 * Before test:                      After test:
 *
 * (src)--->(A)--->(B)--->(dest)     (src)--->(A)--->(B)--->(dest)
 *           └----<-┘
 */
TEST(AbstractARAClientTest, receiveLoopWarning) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("B");
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr source (new AddressMock("src"));
    AddressPtr destination (new AddressMock("dest"));

    Packet* duplicateErrorPacket = new Packet(nodeA, destination, nodeA, PacketType::DUPLICATE_ERROR, 123, 1);

    routingTable->update(destination, destination, interface, 2.2);
    routingTable->update(destination, nodeA, interface, 2.2);

    // Check some basic assumptions about this test setup
    CHECK(routeIsKnown(destination, destination, interface));
    CHECK(routeIsKnown(destination, nodeA, interface));

    // start the test
    client->receivePacket(duplicateErrorPacket, interface);
    CHECK(routeIsKnown(destination, destination, interface));
    CHECK(routeIsKnown(destination, nodeA, interface) == false);
}

TEST(AbstractARAClientTest, deleteAssignedLogger) {
    LoggerMock* logger = new LoggerMock();
    client->setLogger(logger);
    // Should finish without a memory leak
}

/**
 * In this test we want to check that all packets that might still be trapped
 * in the PacketTrap are deleted when the destructor is called.
 */
TEST(AbstractARAClientTest, deleteTrappedPacketsInDestructor) {
    Packet* packet = new PacketMock("source", "destination", 1);

    // the client will trap the packet because it is not deliverable
    client->sendPacket(packet);

    // when the test finishes, the client will be deleted in teardown()
    // and the packet clone should be deleted as well
}

/**
 * In this test we let some ARA client receive a packet that it can directly
 * deliver to its system. Afterwards we attempt to delete this packet.
 * If this leads to a segfault this is a violation of the call semantics of
 * AbstractARAClient::deliverToSystem(..)
 */
TEST(AbstractARAClientTest, packetIsNotDeletedOutsideOfDeliverToSystem) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("destination");
    Packet* dataPacket = new PacketMock("source", "destination", 123, 10, PacketType::DATA);

    client->receivePacket(dataPacket, interface);
    try {
        delete dataPacket;
    } catch (std::exception& exception) {
        FAIL("We should be able to delete the dataPacket.")
    }
}

/**
 * In this test we simulate that a route discovery did not result in a
 * BANT from the wanted destination within a given timeout interval.
 * The client is required to restart the route discovery by sending another
 * FANT for the discovery destination.
 */
TEST(AbstractARAClientTest, routeDiscoveryIsStartedAgainOnTimeout) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    Packet* packet = new Packet(source, destination, source, PacketType::DATA, 1, 10);

    // sanity check
    CHECK(routingTable->isDeliverable(packet) == false);

    // start the test
    client->sendPacket(packet);

    // make sure that a FANT has been sent
    LONGS_EQUAL(1, interface->getNumberOfSentPackets());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->back();
    const Packet* initialFANT = sentPacketInfo->getLeft();
    CHECK(initialFANT->getType() == PacketType::FANT);

    // get the route discovery timer which is used by the client
    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* routeDiscoveryTimer = clock->getLastTimer();

    CHECK(routeDiscoveryTimer->isRunning());

    // simulate that the timer has expired (timeout)
    routeDiscoveryTimer->expire();

    // the timer should have been started again
    CHECK(routeDiscoveryTimer->isRunning());

    // the FANT should have been transmitted again
    BYTES_EQUAL(2, sentPackets->size());
    sentPacketInfo = sentPackets->back();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    CHECK(interface->isBroadcastAddress(recipientOfSentPacket));
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(sentPacket->getDestination()->equals(packet->getDestination()));
}

TEST(AbstractARAClientTest, routeDiscoveryIsAbortedIfToManyTimeoutsOccured) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("source");
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr otherDestination (new AddressMock("otherDestination"));
    Packet* packet1 = new Packet(source, destination, source, PacketType::DATA, 1, 10);
    Packet* packet2 = new Packet(source, destination, source, PacketType::DATA, 2, 10);
    Packet* packet3 = new Packet(source, destination, source, PacketType::DATA, 3, 10);
    Packet* packetToOtherDestiantion = new Packet(source, otherDestination, source, PacketType::DATA, 4, 10);

    int maxNrOfRouteDiscoveryRetries = 4;
    client->setMaxNrOfRouteDiscoveryRetries(maxNrOfRouteDiscoveryRetries);

    // sanity check
    CHECK(routingTable->isDeliverable(packet1->getDestination()) == false);
    CHECK(routingTable->isDeliverable(packetToOtherDestiantion->getDestination()) == false);

    // start the test
    client->sendPacket(packetToOtherDestiantion);
    client->sendPacket(packet1);
    client->sendPacket(packet2);
    client->sendPacket(packet3);

    // none of the packet are deliverable and should be stored in the packet trap
    CHECK(packetTrap->contains(packetToOtherDestiantion));
    CHECK(packetTrap->contains(packet1));
    CHECK(packetTrap->contains(packet2));
    CHECK(packetTrap->contains(packet3));

    // get the route discovery timer which is used by the client
    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* routeDiscoveryTimer = clock->getLastTimer();

    CHECK(routeDiscoveryTimer->isRunning());

    for (int i = 1; i <= maxNrOfRouteDiscoveryRetries; i++) {
        // simulate that the timer has expired (timeout)
        routeDiscoveryTimer->expire();

        // the packet should not have been reported undeliverable yet
        BYTES_EQUAL(0, client->getNumberOfUndeliverablePackets());

        // the timer should have been started again
        CHECK(routeDiscoveryTimer->isRunning());
    }

    // expire one last time
    routeDiscoveryTimer->expire();

    // now we have tried so many times and should abort the route discovery
    BYTES_EQUAL(3, client->getNumberOfUndeliverablePackets());

    // the undeliverable packets must be deleted from the trap
    CHECK_TRUE(packetTrap->contains(packetToOtherDestiantion));
    CHECK_FALSE(packetTrap->contains(packet1));
    CHECK_FALSE(packetTrap->contains(packet2));
    CHECK_FALSE(packetTrap->contains(packet3));
}

/**
 * In this test we check if a client creates a routing table entry to one of its own interface
 * addresses if it receives back one of its FANTs or BANTs.
 */
TEST(AbstractARAClientTest, doNotSaveRoutesToSelf) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("source");
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    Packet* fant = packetFactory->makeFANT(source, destination, 123);
    fant->setPreviousHop(source);

    // sanity check
    CHECK(routingTable->isDeliverable(source) == false);

    // start the test
    client->receivePacket(fant, interface);

    // we should still not know this address in the routing table
    CHECK(routingTable->isDeliverable(source) == false);
}

/**
 * In this test we check that the path to the source is reinforced.
 *
 * Test Topology:
 *
 *     (A)-->(B)-->(C)-->(D)
 *                  |
 *                  --current (tested) node
 */
TEST(AbstractARAClientTest, pathToSourceIsReinforced) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("C");
    AddressPtr source(new AddressMock("A"));
    AddressPtr sender(new AddressMock("B"));
    AddressPtr destination(new AddressMock("D"));
    int maxHopCount = 10;

    // sanity check
    CHECK_FALSE(routeIsKnown(source, sender, interface));

    // start the test
    Packet* fant = new Packet(source, destination, sender, PacketType::FANT, 123, maxHopCount);
    client->receivePacket(fant, interface);

    // first of all a route should have been created
    CHECK_TRUE(routeIsKnown(source, sender, interface));
    float currentPhi = routingTable->getPheromoneValue(source, sender, interface);

    // now we receive the first data packet
    Packet* data = new Packet(destination, source, destination, PacketType::DATA, 124, maxHopCount);
    client->receivePacket(data, interface);

    // this should reinforce the path to the source
    float newPhi = routingTable->getPheromoneValue(source, sender, interface);
    CHECK(newPhi > currentPhi);
}

/**
 * In this test we check that the path to the destination is reinforced.
 *
 * Test Topology:
 *
 *     (A)<--(X)<--(B)-->(C)
 *            |
 *            --current (tested) node
 */
TEST(AbstractARAClientTest, pathToDestinationIsReinforced) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("X");
    AddressPtr source(new AddressMock("A"));
    AddressPtr sender(new AddressMock("B"));
    AddressPtr destination(new AddressMock("C"));
    int maxHopCount = 10;

    // sanity check
    CHECK_FALSE(routeIsKnown(destination, sender, interface));

    // start the test
    Packet* bant = new Packet(destination, source, sender, PacketType::BANT, 123, maxHopCount);
    client->receivePacket(bant, interface);

    // first of all a route should have been created
    CHECK_TRUE(routeIsKnown(destination, sender, interface));
    float currentPhi = routingTable->getPheromoneValue(destination, sender, interface);

    // now we receive a data packet
    Packet* data = new Packet(source, destination, source, PacketType::DATA, 124, maxHopCount);
    client->receivePacket(data, interface);

    // this should reinforce the path to the destination
    float newPhi = routingTable->getPheromoneValue(destination, sender, interface);
    CHECK(newPhi > currentPhi);
}

/**
 * Test setup:                           | Description:
 * (source)---(A)---(B*)---(C)---(dest)  |   * We test from the perspective of node B
 *                                       |
 */
TEST(AbstractARAClientTest, pathToDestinationEvaporates) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("B");
    AddressPtr source(new AddressMock("source"));
    AddressPtr destination(new AddressMock("dest"));
    AddressPtr nodeA(new AddressMock("A"));
    AddressPtr nodeC(new AddressMock("C"));
    int maxHopCount = 10;

    // sanity check
    CHECK(routeIsKnown(source, nodeA, interface) == false);
    CHECK(routeIsKnown(destination, nodeC, interface) == false);

    // receive FANT: (source)-->(A)-->(B*)-->(C)-->(dest)
    Packet* fant = new Packet(source, destination, nodeA, PacketType::FANT, 123, maxHopCount);
    client->receivePacket(fant, interface);
    CHECK(routeIsKnown(source, nodeA, interface));

    // receive BANT: (source)<--(A)<--(B*)<--(C)<--(dest)
    Packet* bant = new Packet(destination, source, nodeC, PacketType::BANT, 124, maxHopCount);
    client->receivePacket(bant, interface);
    CHECK(routeIsKnown(destination, nodeC, interface));

    float oldPhiToSrc = routingTable->getPheromoneValue(source, nodeA, interface);
    float oldPhiToDest = routingTable->getPheromoneValue(destination, nodeC, interface);

    // receive DATA: (source)-->(A)-->(B*)-->(C)-->(dest)
    Packet* data = new Packet(source, destination, nodeA, PacketType::DATA, 125, maxHopCount);
    client->receivePacket(data, interface);

    // check if the reinforcement has worked on both sides of the route
    float newPhiToSrc = routingTable->getPheromoneValue(source, nodeA, interface);
    float newPhiToDest = routingTable->getPheromoneValue(destination, nodeC, interface);

    CHECK(newPhiToSrc > oldPhiToSrc);
    CHECK(newPhiToDest > oldPhiToDest);

    // update the values so we can check the evaporation
    oldPhiToDest = newPhiToDest;
    oldPhiToSrc = newPhiToSrc;

    TimeMock::letTimePass(2000);
    routingTable->triggerEvaporation();

    newPhiToDest = routingTable->getPheromoneValue(destination, nodeC, interface);
    newPhiToSrc = routingTable->getPheromoneValue(source, nodeA, interface);

    // check if the evaporation has taken place
    CHECK(newPhiToDest < oldPhiToDest);
    CHECK(newPhiToSrc < oldPhiToSrc);
}

/**
 * In this test we check if a client who knows two (or more routes) to a destination is
 * able to fall back to another route if one of the known routes is failing.
 */
TEST(AbstractARAClientTest, takeAlternativeRouteInRouteFailure) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("source");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender = source;
    AddressPtr destination (new AddressMock("destination"));
    unsigned int originalSeqNr = 123;
    int originalTTL = 5;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, originalSeqNr, originalTTL, "Foo", 4);
    AddressPtr route1 (new AddressMock("route1"));
    AddressPtr route2 (new AddressMock("route2"));

    // create two known routes to the destination
    routingTable->update(destination, route1, interface, 20);
    routingTable->update(destination, route2, interface, 10);

    // sanity check
    CHECK(routingTable->isDeliverable(destination));
    CHECK(sentPackets->empty());

    // start the test
    client->handleBrokenLink(packet, route1, interface);

    // the client is expected to delete the route from the routing table
    CHECK(routingTable->exists(destination, route1, interface) == false);

    // the packet should have been sent via the other route
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(sentPacketInfo->getRight()->equals(route2));

    CHECK_PACKET(sentPacketInfo->getLeft(), PacketType::DATA, originalSeqNr, originalTTL, source, sender, destination, "Foo");
}

TEST(AbstractARAClientTest, broadcastRouteFailureIfNoAlternativeRoutesAreKownOnRouteFailure) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("sender");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, 123, 10);
    AddressPtr nextHop (new AddressMock("nextHop"));
    int maxNrOfHops = packetFactory->getMaximumNrOfHops();

    // create a known route to the destination
    routingTable->update(destination, nextHop, interface, 10);

    // sanity check
    CHECK(routingTable->isDeliverable(destination));
    CHECK(sentPackets->empty());

    // start the test
    client->handleBrokenLink(packet, nextHop, interface);

    // the client is expected to delete the route from the routing table
    CHECK(routingTable->exists(destination, nextHop, interface) == false);

    // the client should have sent a broadcast indicating that he can no longer relay to that destination
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(interface->isBroadcastAddress(sentPacketInfo->getRight()));
    CHECK(sentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(sentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(sentPacket->getDestination()->equals(destination));
    BYTES_EQUAL(maxNrOfHops, sentPacket->getTTL());
}

TEST(AbstractARAClientTest, clientsDeleteRoutingTableEntryWhenTheyReceiveRoutingFailurePacket) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("sender");
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination (new AddressMock("destination"));
    Packet* routeFailurePacket = new Packet(source, destination, sender, PacketType::ROUTE_FAILURE, 123, 10);

    routingTable->update(destination, sender, interface, 10);

    // sanity check
    CHECK(routingTable->exists(destination, sender, interface));

    // start the test
    client->receivePacket(routeFailurePacket, interface);

    // the client should have deleted the entry from its routing table
    CHECK(routingTable->exists(destination, sender, interface) == false);
}

/**
 * TODO This test checks if issue #25 has been resolved
 */
IGNORE_TEST(AbstractARAClientTest, doSendDuplicateWarningToSameSender) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("source");
    SendPacketsList* sentPackets = interface->getSentPackets();

    unsigned int sequenceNumber = 123;
    Packet* originalPacket = new PacketMock("source", "destination", "sender", sequenceNumber);
    Packet* clone = packetFactory->makeClone(originalPacket);

    // start the test
    client->receivePacket(originalPacket, interface);

    // we should have sent an acknowledgment for the first packet
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->back();
    const Packet* firstAck = sentPacketInfo->getLeft();
    CHECK(firstAck->getType() == PacketType::ACK);

    // receive a packet with the same source and sequence number again from the same sender
    client->receivePacket(clone, interface);

    // check that we have just received another ACK and no duplicate warning
    BYTES_EQUAL(2, sentPackets->size());
    sentPacketInfo = sentPackets->back();
    const Packet* secondAck = sentPacketInfo->getLeft();
    CHECK(secondAck->getType() == PacketType::ACK);
}

/**
 * Simply check if a node remembers multiple routes to a specific destination.
 *
 *        /--(A)-------\
 * (source)            (testNode)--(destination)
 *        \--(B)--(C)--/
 *
 */
TEST(AbstractARAClientTest, rememberMultipleRoutes) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("testNode");
    AddressPtr source (new AddressMock("source"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr destination (new AddressMock("destination"));

    // create packet from (source) via (A)
    Packet* packetFromA = new Packet(source, destination, nodeA, PacketType::FANT, 123, 10);

    // packet from (C) is the same but differs in the sender and prevHop
    Packet* packetFromC = packetFactory->makeClone(packetFromA);
    packetFromC->setPreviousHop(nodeB);
    packetFromC->setSender(nodeC);

    // sanity check
    CHECK_FALSE(routeIsKnown(source, nodeA, interface));
    CHECK_FALSE(routeIsKnown(source, nodeC, interface));

    // start the test
    client->receivePacket(packetFromA, interface);
    CHECK_TRUE(routeIsKnown(source, nodeA, interface));
    CHECK_FALSE(routeIsKnown(source, nodeC, interface));

    client->receivePacket(packetFromC, interface);
    CHECK_TRUE(routeIsKnown(source, nodeA, interface));
    CHECK_TRUE(routeIsKnown(source, nodeC, interface));
}

TEST(AbstractARAClientTest, doNotRelayPacketIfTTLBecomesZero) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();

    AddressPtr source (new AddressMock("B"));
    AddressPtr destination (new AddressMock("C"));
    AddressPtr sender = source;
    int ttl = 1; // only one last hop allowed
    Packet* data = new Packet(source, destination, sender, PacketType::DATA, 1, ttl, "Hello World");
    Packet* fant = new Packet(source, destination, sender, PacketType::FANT, 2, ttl);
    Packet* bant = new Packet(source, destination, sender, PacketType::BANT, 3, ttl);

    // create a route to the destination so the packet could theoretically be relayed
    routingTable->update(destination, destination, interface, 10);

    // start the test
    client->receivePacket(data, interface);
    BYTES_EQUAL(0, sentPackets->size());

    client->receivePacket(fant, interface);
    BYTES_EQUAL(0, sentPackets->size());

    client->receivePacket(bant, interface);
    BYTES_EQUAL(0, sentPackets->size());
}

TEST(AbstractARAClientTest, initialzePheromoneValue) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("X");
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr route1 (new AddressMock("1"));
    AddressPtr route2 (new AddressMock("2"));
    AddressPtr route3 (new AddressMock("3"));
    AddressPtr route4 (new AddressMock("4"));

    int ttl1 = 10;
    int ttl2 = 8;
    int ttl3 = 5;
    int ttl4 = 1;
    Packet* fant1 = new Packet(source, destination, route1, PacketType::FANT, 123, ttl1);
    Packet* fant2 = new Packet(source, destination, route2, PacketType::FANT, 123, ttl2);
    Packet* fant3 = new Packet(source, destination, route3, PacketType::FANT, 123, ttl3);
    Packet* fant4 = new Packet(source, destination, route4, PacketType::FANT, 123, ttl4);

    // sanity check
    CHECK_FALSE(routingTable->isDeliverable(destination));

    // start test
    double initialPhi = client->getInitialPhi();
    client->receivePacket(fant1, interface);
    DOUBLES_EQUAL(1 * (ttl1) + initialPhi, routingTable->getPheromoneValue(source, route1, interface), 0.000001);

    client->receivePacket(fant2, interface);
    DOUBLES_EQUAL(1 * (ttl2) + initialPhi, routingTable->getPheromoneValue(source, route2, interface), 0.000001);

    client->receivePacket(fant3, interface);
    DOUBLES_EQUAL(1 * (ttl3) + initialPhi, routingTable->getPheromoneValue(source, route3, interface), 0.000001);

    client->receivePacket(fant4, interface);
    DOUBLES_EQUAL(1 * (ttl4) + initialPhi, routingTable->getPheromoneValue(source, route4, interface), 0.000001);
}

/**
 * In this test we check that a client sets the address of the node from which
 * it received a packet as previous hop for the relayed packet.
 *
 * We test as Node (C)
 * 1.                    2.
 *  (A)-->(B)--p-->(C)    (A)-->(B)-->(C)--p-->(D)
 *         |                     |
 *         └ sender              └ now this is the previous hop for node (D)
 */
TEST(AbstractARAClientTest, addPreviousHopToPacket) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("C");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeD (new AddressMock("D"));

    // first test this with a FANT
    Packet* fant = new Packet(nodeA, nodeD, nodeB, PacketType::FANT, 1, 10);
    client->receivePacket(fant, interface);

    // check the previous hop of the relayed packet
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(sentPacket->getPreviousHop()->equals(nodeB));

    // the same should work if the route has been established and a DATA packet is relayed
    Packet* data = new Packet(nodeD, nodeA, nodeD, PacketType::DATA, 2, 10);
    client->receivePacket(data, interface);

    BYTES_EQUAL(2, sentPackets->size());
    sentPacketInfo = sentPackets->back();
    sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getType() == PacketType::DATA);
    CHECK(sentPacket->getPreviousHop()->equals(nodeD));
}

/**
 * In this test we want to check that a client does not record a route to a destination (d)
 * if he already knows a route to (d) which goes over the same previous hop (A).
 *
 * Test setup:                   | Description:
 * (d)<--(A)<--(B)<--(...)       |   * (B) and (C) know a route to (d) via (A) from a previous broadcast
 *        |     |                |   * if either (B) or (C) broadcast a FANT, each of the other will receive it,
 *        |     |                |     but should not create the route via the other.
 *        └-<--(C)<--(...)       |   * this means, there shall not be the route (C)->(B)->(A)->(d) or
 *                               |                                              (B)->(C)->(A)->(d)
 *                               |   * the only allowed routes are:  (B)->(A)->(d) and
 *                               |                                   (C)->(A)->(d)
 */
TEST(AbstractARAClientTest, noRouteOverPreviousHop) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("C");
    AddressPtr nodeD (new AddressMock("d"));
    AddressPtr destination (new AddressMock("..."));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));

    // we test from the perspective of node C
    CHECK(routingTable->isDeliverable(nodeD) == false);

    // at first we receive the broadcast from A
    Packet* fantFromA = new Packet(nodeD, destination, nodeA, PacketType::FANT, 1, 10);
    client->receivePacket(fantFromA, interface);

    // this should have created a route to (d) via (A)
    CHECK_TRUE(routeIsKnown(nodeD, nodeA, interface));

    // now we receive the broadcast from (B)
    Packet* fantFromB = new Packet(nodeD, destination, nodeB, PacketType::FANT, 1, 9);
    fantFromB->setPreviousHop(nodeA);
    client->receivePacket(fantFromB, interface);

    // this should *not* create the route to (d) via (B)
    CHECK_FALSE(routeIsKnown(nodeD, nodeB, interface));
}

/**
 * In this test we want to check that a client does not record a route to a destination (d)
 * if he one of his own interface addresses equals the previous hop of the received packet.
 * This will prevent the host from creating routes that will lead over him self which would
 * create a loop.
 *
 * Test setup:                   | Description:
 * (src)<--(A)<--(B)<--(dest)    |   * We test from the perspective of node (A)

 */
TEST(AbstractARAClientTest, doNotCreateRouteOverSelf) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    AddressPtr source (new AddressMock("src"));
    AddressPtr destination (new AddressMock("dest"));
    AddressPtr nodeB (new AddressMock("B"));

    // lets assume we have already broadcasted a FANT to (B).
    // we now simulate that (B) rebroadcasts this FANT again and we receive this packet as well
    Packet* fantFromB =  new Packet(source, destination, nodeB, PacketType::FANT, 1, 10);
    // of course (A) is now the previous hop from the perspective of (B)
    fantFromB->setPreviousHop(interface->getLocalAddress());

    // start the test
    client->receivePacket(fantFromB, interface);

    // we do *not* want to remember this route over the own address
    CHECK(routeIsKnown(source, nodeB, interface) == false);
}

/**
 * In this test we check that a client waits the configured amount of time before
 * it actually starts to send DATA packets after it received the first BANT from
 * the destination.
 */
TEST(AbstractARAClientTest, clientWaitsBeforeSendingTheDATA) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("S");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("D"));
    AddressPtr nodeI (new AddressMock("i"));
    AddressPtr nodeJ (new AddressMock("j"));

    // sanity check
    Packet* packet = new Packet(source, destination, source, PacketType::DATA, 123, 10);
    CHECK(routingTable->isDeliverable(packet) == false);

    // start the test
    client->sendPacket(packet);

    // should have triggered a new route discovery
    BYTES_EQUAL(1, sentPackets->size());
    const Packet* sentPacket = sentPackets->back()->getLeft();
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(packetTrap->contains(packet) == true);

    // receive the first BANT back from the destination
    Packet* bant1 =  new Packet(destination, source, nodeI, PacketType::BANT, 1, 10);
    client->receivePacket(bant1, interface);
    CHECK(routingTable->isDeliverable(packet) == true);

    // the client is required to wait some time for other BANTs coming back from D until it starts sending packets
    BYTES_EQUAL(1, sentPackets->size());

    // Hurray! another BANT just came back so we have now two available routes :)
    Packet* bant2 =  new Packet(destination, source, nodeJ, PacketType::BANT, 1, 10);
    client->receivePacket(bant2, interface);

    // Still the timer has not expired yet so nothing should have been sent
    BYTES_EQUAL(1, sentPackets->size());

    // Let the send timer expire
    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* sendTimer = clock->getLastTimer();
    sendTimer->expire();
    BYTES_EQUAL(2, sentPackets->size());
    CHECK(packetTrap->isEmpty());

    // lets check the last send packet
    sentPacket = sentPackets->back()->getLeft();
    CHECK(sentPacket->getType() == PacketType::DATA);
    BYTES_EQUAL(123, sentPacket->getSequenceNumber());
}

/**
 * This test checks if the route failure mechanism works correctly
 *
 * Test setup:                   | Description:
 * (src)---(A)---(B)   (dest)    |   * B receives a packet to dest from A
 *                               |   * B has no route so it broadcasts a ROUTE_FAILURE
 *                               |   * A should delete the route via B
 */
TEST(AbstractARAClientTest, routeFailurePacketIsCorrectlyProcessed) {
    ARAClientMock* nodeA = client;
    ARAClientMock nodeB = ARAClientMock();
    NetworkInterfaceMock* interfaceOfA = nodeA->createNewNetworkInterfaceMock("A");
    NetworkInterfaceMock* interfaceOfB = nodeB.createNewNetworkInterfaceMock("B");
    SendPacketsList* sentPacketsOfA = interfaceOfA->getSentPackets();
    SendPacketsList* sentPacketsOfB = interfaceOfB->getSentPackets();

    AddressPtr addressOfnodeA = interfaceOfA->getLocalAddress();
    AddressPtr addressOfnodeB = interfaceOfB->getLocalAddress();
    AddressPtr destination (new AddressMock("dest"));
    AddressPtr source (new AddressMock("src"));

    routingTable->update(destination, addressOfnodeB, interfaceOfA, 2);

    // sanity check
    CHECK_TRUE(routeIsKnown(destination, addressOfnodeB, interfaceOfA));
    CHECK_FALSE(nodeB.getRoutingTable()->isDeliverable(destination));

    Packet* dataPacket = new Packet(source, destination, source, PacketType::DATA, 1, 10);

    // start the test
    nodeA->receivePacket(dataPacket, interfaceOfA);

    // (A) should have relayed the packet to B
    BYTES_EQUAL(1, sentPacketsOfA->size())
    const Packet* sentPacket1FromA = sentPacketsOfA->back()->getLeft();
    CHECK(sentPacket1FromA->getType() == PacketType::DATA);
    CHECK(sentPacketsOfA->back()->getRight()->equals(addressOfnodeB));

    nodeB.receivePacket(packetFactory->makeClone(sentPacket1FromA), interfaceOfB);

    // (B) should have sent the route failure packet
    BYTES_EQUAL(1, sentPacketsOfB->size())
    const Packet* sentPacket1FromB = sentPacketsOfB->back()->getLeft();
    CHECK(sentPacket1FromB->getType() == PacketType::ROUTE_FAILURE);

    // now (A) receives the route failure from (B)
    nodeA->receivePacket(packetFactory->makeClone(sentPacket1FromB), interfaceOfA);

    // the route via (B) should now have been removed
    CHECK_FALSE(routeIsKnown(destination, addressOfnodeB, interfaceOfA));
}

/**
 * In this test we check that packets are trapped if there is already an ongoing
 * route discovery for this destination
 */
TEST(AbstractARAClientTest, routeDiscoveryIsNotStartedTwice) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    SendPacketsList* sentPackets = interface->getSentPackets();

    // start the test
    Packet* packet1 = new Packet(source, destination, source, PacketType::DATA, 1, 10);
    client->sendPacket(packet1);

    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* routeDiscoveryTimer = clock->getLastTimer();

    // a route discovery should have been started for packet1
    CHECK(routeDiscoveryTimer->isRunning());

    // the packet is trapped until delivery
    CHECK(packetTrap->contains(packet1));

    // check if the FANT has been sent just to be sure
    BYTES_EQUAL(1, interface->getNumberOfSentPackets());
    const Packet* sentPacket1 = sentPackets->back()->getLeft();
    CHECK(sentPacket1->getType() == PacketType::FANT);
    CHECK(sentPacket1->getDestination()->equals(destination));

    // now send another packet to that destination and see if it is also trapped
    Packet* packet2 = new Packet(source, destination, source, PacketType::DATA, 2, 10);
    client->sendPacket(packet2);

    // this packet needs to be stored until the delivery timer expires
    CHECK(packetTrap->contains(packet2));

    // the client should *not* have started a new route discovery so still only one FANT has been sent
    BYTES_EQUAL(1, interface->getNumberOfSentPackets());
}

/**
 * In this test two packets shall be delivered to one destination.
 * For the first one a route discovery has just finished but the delivery timer is still
 * running. The second packet shall not start a new discovery now because it will be
 * delivered as well as soon as the delivery timer times out.
 */
TEST(AbstractARAClientTest, routeDiscoveryIsNotStartedTwiceSpecialCase) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    SendPacketsList* sentPackets = interface->getSentPackets();

    // start the test
    Packet* packet1 = new Packet(source, destination, source, PacketType::DATA, 1, 10);
    client->sendPacket(packet1);

    ClockMock* clock = (ClockMock*) Environment::getClock();
    TimerMock* routeDiscoveryTimer = clock->getLastTimer();

    // a route discovery should have been started for packet1
    CHECK(routeDiscoveryTimer->isRunning());

    // the packet is trapped until delivery
    CHECK(packetTrap->contains(packet1));

    // check if the FANT has been sent just to be sure
    BYTES_EQUAL(1, interface->getNumberOfSentPackets());
    const Packet* sentPacket1 = sentPackets->back()->getLeft();
    CHECK(sentPacket1->getType() == PacketType::FANT);
    CHECK(sentPacket1->getDestination()->equals(destination));

    // the client now receives the BANT which should end the route discovery timer and start the delivery timer
    Packet* bant = new Packet(destination, source, destination, PacketType::BANT, 1, 10);
    client->receivePacket(bant, interface);

    // now a delivery timer should be running as long as the client waits for other BANTs to come back from the destination
    TimerMock* deliveryTimer = clock->getLastTimer();

    // meanwhile the upper layer has generated another packet to send to the destination
    Packet* packet2 = new Packet(source, destination, source, PacketType::DATA, 2, 10);
    client->sendPacket(packet2);

    // this packet needs to be stored until the delivery timer expires
    CHECK(packetTrap->contains(packet2));

    // the client should *not* have started a new route discovery so still only one FANT has been sent
    BYTES_EQUAL(1, interface->getNumberOfSentPackets());

    // also no new timer could have been started
    CHECK(clock->getLastTimer() == deliveryTimer);

    // both packets are sent together when the delivery timer expires
    deliveryTimer->expire();

    BYTES_EQUAL(1 + 2, interface->getNumberOfSentPackets());
    CHECK(packet1->equals(sentPackets->at(1)->getLeft()) || packet1->equals(sentPackets->at(2)->getLeft()));
    CHECK(packet2->equals(sentPackets->at(1)->getLeft()) || packet2->equals(sentPackets->at(2)->getLeft()));
}

/**
 * In this test we check that a client which is the source of a transmission does not drop a packet if all
 * his known routes do no longer work. Instead it is required to trap the packet and start a new route discovery.
 */
TEST(AbstractARAClientTest, restartRouteDiscoveryIfSourceHasCompleteRouteFailure) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr nextHopA (new AddressMock("nextHopA"));
    AddressPtr nextHopB (new AddressMock("nextHopB"));

    routingTable->update(destination, nextHopA, interface, 10);
    routingTable->update(destination, nextHopB, interface, 10);

    // start the test by faking that the packet has been send via nextHopA and this link is broken
    Packet* packet = new Packet(source, destination, source, PacketType::DATA, 1, 10);
    client->handleBrokenLink(packet, nextHopA, interface);

    // the client should try to deliver the packet over the remaining known route
    BYTES_EQUAL(1, interface->getNumberOfSentPackets());
    const Packet* lastSentPacket = sentPackets->back()->getLeft();
    AddressPtr receiverAddress = sentPackets->back()->getRight();
    CHECK(lastSentPacket->equals(packet));
    CHECK(receiverAddress->equals(nextHopB));

    // unfortunately nextHopB is also not reachable
    Packet* samePacket = packetFactory->makeClone(packet);
    client->handleBrokenLink(samePacket, nextHopB, interface);

    // now we require the client to start a new route discovery
    CHECK(packetTrap->contains(samePacket));
    BYTES_EQUAL(2, interface->getNumberOfSentPackets());
    lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::FANT);
}
