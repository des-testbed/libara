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
#include "TimerType.h"

#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/RoutingTableMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/LoggerMock.h"
#include "testAPI/mocks/time/ClockMock.h"

#include <iostream>

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

    /**
     * This is just for debugging failing tests
     */
    void printAllSentPackets(NetworkInterfaceMock* interface) {
        printAllSentPackets(interface->getSentPackets());
    }

    /**
     * This is just for debugging failing tests
     */
    void printAllSentPackets(SendPacketsList* sentPackets) {
        std::cout << std::endl << "DEBUG: Client has sent " << sentPackets->size() << " packets" << std::endl;
        int packetCounter = 0;
        for (auto& sendPacketPair: *sentPackets) {
            packetCounter++;
            const Packet* packet = sendPacketPair->getLeft();
            AddressPtr receiver = sendPacketPair->getRight();

            std::cout << packetCounter << ": " << PacketType::getAsString(packet->getType());
            std::cout << " from " << packet->getSourceString() << " to " << packet->getDestinationString() << " via " << receiver->toString() << std::endl;
        }
    }

    void activateNeighborActivityCheck(unsigned int maxNeighborInavtivityTimeInMs, unsigned int neighborCheckIntervalInMs = 500) {
        BasicConfiguration configuration = client->getStandardConfiguration();
        configuration.setNeighborActivityCheckInterval(neighborCheckIntervalInMs); // this doesn't really matter in the tests as the timer is usually expired manually by the test
        configuration.setMaxNeighborInactivityTime(maxNeighborInavtivityTimeInMs);

        createNewClient(configuration);
    }

    void createNewClient(Configuration& configuration) {
        // first delete the old client
        delete client;

        // then create a new one
        client = new ARAClientMock(configuration);
        packetTrap = client->getPacketTrap();
        routingTable = (RoutingTableMock*) client->getRoutingTable();
        packetFactory = client->getPacketFactory();
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
    NetworkInterfaceMock interface1 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface2 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface3 = NetworkInterfaceMock(client);
    CHECK_EQUAL(0, client->getNumberOfNetworkInterfaces());

    client->addNetworkInterface(&interface1);
    CHECK_EQUAL(1, client->getNumberOfNetworkInterfaces());

    client->addNetworkInterface(&interface2);
    CHECK_EQUAL(2, client->getNumberOfNetworkInterfaces());

    client->addNetworkInterface(&interface3);
    CHECK_EQUAL(3, client->getNumberOfNetworkInterfaces());
}

TEST(AbstractARAClientTest, getNetworkInterface) {
    NetworkInterfaceMock interface1 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface2 = NetworkInterfaceMock(client);
    NetworkInterfaceMock interface3 = NetworkInterfaceMock(client);
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
    std::shared_ptr<TimerMock> routeDiscoveryTimer = (clock->getLastTimer()).lock();

    CHECK(routeDiscoveryTimer->getType() == TimerType::ROUTE_DISCOVERY_TIMER);
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
    std::shared_ptr<TimerMock> routeDiscoveryTimer = (clock->getLastTimer()).lock();

    CHECK(routeDiscoveryTimer->getType() == TimerType::ROUTE_DISCOVERY_TIMER);
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
    AddressPtr route3 (new AddressMock("route3"));

    // create three known routes to the destination
    routingTable->update(destination, route1, interface, 20);
    routingTable->update(destination, route2, interface, 10);
    routingTable->update(destination, route3, interface, 10);

    // sanity check
    CHECK(routingTable->isDeliverable(destination));
    CHECK(sentPackets->empty());

    // start the test
    client->handleBrokenLink(packet, route1, interface);

    // the client is expected to delete the route from the routing table
    CHECK(routingTable->exists(destination, route1, interface) == false);

    // the packet should have been sent via one of the other routes
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    AddressPtr receiver = sentPacketInfo->getRight();
    CHECK(receiver->equals(route2) || receiver->equals(route3));

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
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination (new AddressMock("destination"));
    Packet* routeFailurePacket = packetFactory->makeRouteFailurePacket(sender, destination, 123);

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
    std::shared_ptr<TimerMock> sendTimer = (clock->getLastTimer()).lock();

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
    std::shared_ptr<TimerMock> routeDiscoveryTimer = (clock->getLastTimer()).lock();

    // a route discovery should have been started for packet1
    CHECK(routeDiscoveryTimer->getType() == TimerType::ROUTE_DISCOVERY_TIMER);
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
    std::shared_ptr<TimerMock> routeDiscoveryTimer = (clock->getLastTimer()).lock();


    // a route discovery should have been started for packet1
    CHECK(routeDiscoveryTimer->getType() == TimerType::ROUTE_DISCOVERY_TIMER);
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
    std::shared_ptr<TimerMock> deliveryTimer = (clock->getLastTimer()).lock();

    // meanwhile the upper layer has generated another packet to send to the destination
    Packet* packet2 = new Packet(source, destination, source, PacketType::DATA, 2, 10);
    client->sendPacket(packet2);

    // this packet needs to be stored until the delivery timer expires
    CHECK(packetTrap->contains(packet2));

    // the client should *not* have started a new route discovery so still only one FANT has been sent
    BYTES_EQUAL(1, interface->getNumberOfSentPackets());

    // also no new timer could have been started
    CHECK((clock->getLastTimer()).lock() == deliveryTimer);

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

    // At first the client notifies the last remaining neighbor that it can no longer send over this client as this would lead to a loop
    BYTES_EQUAL(2, interface->getNumberOfSentPackets());
    const Packet* sentPacket = sentPackets->at(0)->getLeft();
    AddressPtr receiverAddress = sentPackets->at(0)->getRight();
    CHECK(sentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(receiverAddress->equals(nextHopB));

    // Then it should try to deliver the packet over the remaining known route
    sentPacket = sentPackets->at(1)->getLeft();
    receiverAddress = sentPackets->at(1)->getRight();
    CHECK(sentPacket->equals(packet));
    CHECK(receiverAddress->equals(nextHopB));

    // unfortunately nextHopB is also not reachable
    Packet* samePacket = packetFactory->makeClone(packet);
    client->handleBrokenLink(samePacket, nextHopB, interface);

    // now we require the client to recognize the complete route failure and start a new route discovery
    CHECK(packetTrap->contains(samePacket));
    BYTES_EQUAL(4, interface->getNumberOfSentPackets());
    sentPacket = sentPackets->at(2)->getLeft();
    receiverAddress = sentPackets->at(2)->getRight();
    CHECK(sentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(interface->isBroadcastAddress(receiverAddress));

    sentPacket = sentPackets->at(3)->getLeft();
    receiverAddress = sentPackets->at(3)->getRight();
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(interface->isBroadcastAddress(receiverAddress));
}

/**
 * In this test we consider the scenario that a node wants to send a packet but notices that
 * all known routes are no longer reachable (broken link) and tries to restart the
 * route discovery (like in the test above).
 * This time the clients gets the handleBorken link message twice for different packets.
 * It is required that only one route discovery is started for the first packet
 * The seconds packet must be trapped and shall not cause another route discovery
 */
TEST(AbstractARAClientTest, routeDiscoveryIsNotStartedTwiceIfSourceHasCompleteRouteFailure) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr nextHop (new AddressMock("nextHopA"));

    routingTable->update(destination, nextHop, interface, 10);

    // start the test by faking that the packet has been send via nextHop and this link is broken
    Packet* packet1 = new Packet(source, destination, source, PacketType::DATA, 1, 10);
    client->handleBrokenLink(packet1, nextHop, interface);

    // there are no other routes left so the client should broadcast a ROUTE_FAILURE
    BYTES_EQUAL(2, interface->getNumberOfSentPackets());
    const Packet* sentPacket = sentPackets->at(0)->getLeft();
    AddressPtr receiverAddress = sentPackets->at(0)->getRight();
    CHECK(sentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(interface->isBroadcastAddress(receiverAddress));

    // now since this is the source of the packet the client should start a route discovery
    sentPacket = sentPackets->at(1)->getLeft();
    receiverAddress = sentPackets->at(1)->getRight();
    CHECK(sentPacket->getType() == PacketType::FANT);
    CHECK(interface->isBroadcastAddress(receiverAddress));
    CHECK(packetTrap->contains(packet1));

    // meanwhile the delivery of another packet failed
    Packet* packet2 = new Packet(source, destination, source, PacketType::DATA, 2, 10);
    client->handleBrokenLink(packet2, nextHop, interface);

    // this should never cause another route discovery as there is already a running discovery for that destination
    BYTES_EQUAL(2, interface->getNumberOfSentPackets()); // still only 2 packets have been sent
    CHECK(packetTrap->contains(packet2));
}

/**
 * In this test we check that a client broadcasts another ROUTE_FAILURE to all of its neighbors
 * if it just deleted the last known route to a destination due to a previously received ROUTE_FAILURE
 * from one of its neighbors
 *
 * Test setup:                   | Description:
 *                               |   * We are testing from the perspective of (A)
 * (...)---(A)---(B)   (dest)    |   * A receives a ROUTE_FAILURE packet from (B)
 *                               |   * first (A) deletes its only route do (dest) via (B)
 *                               |   * (A) has no other route so it also broadcasts a ROUTE_FAILURE
 */
TEST(AbstractARAClientTest, brodcastRouteFailureIfAllAvailableroutesHaveBeenDeletedDueToRoutingFailure) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr destination (new AddressMock("destination"));

    // we are testing from the perspective of (A)
    routingTable->update(destination, nodeB, interface, 10);

    // start the test by receiving the ROUTE_FAILURE packet from (B)
    Packet* routeFailure = packetFactory->makeRouteFailurePacket(nodeB, destination, 1);
    client->receivePacket(routeFailure, interface);

    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    BYTES_EQUAL(1, sentPackets->size());

    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(interface->isBroadcastAddress(receiver));

    const Packet* lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(lastSentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(lastSentPacket->getDestination()->equals(destination));
}

/**
 * This test checks if a client sends the ROUTE_FAILURE to a neighbor
 * if it has only one single next hop available for a specific destination.
 *
 * Test setup:                     | Description:
 *                                 |   * We are testing from the perspective of (A)
 * (...)---(A)-->(B)-(...)--(dest) |   * A receives a ROUTE_FAILURE packet from (B)
 *          └--->(C)---------/     |   * first (A) deletes the route do (dest) via (B)
 *                                 |   * then is recognizes that the only remaining route leads via (C)
 *                                 |   * (A) send a ROUTE_FAILURE to (C) to prevent it from sending any traffic to (dest) via (A)
 */
TEST(AbstractARAClientTest, sendRouteFailureIfOnlyOneRouteIsLeftDueToRoutingFailure) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr destination (new AddressMock("destination"));

    // we are testing from the perspective of (A)
    routingTable->update(destination, nodeB, interface, 10);
    routingTable->update(destination, nodeC, interface, 10);

    // start the test by receiving the ROUTE_FAILURE packet from (B)
    Packet* routeFailure = packetFactory->makeRouteFailurePacket(nodeB, destination, 1);
    client->receivePacket(routeFailure, interface);

    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_TRUE( routingTable->exists(destination, nodeC, interface));
    BYTES_EQUAL(1, sentPackets->size());

    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(receiver->equals(nodeC));

    const Packet* lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(lastSentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(lastSentPacket->getDestination()->equals(destination));
}

/**
 * In this test we check that a client broadcasts a ROUTE_FAILURE to all of its neighbors
 * if it just deleted the last known route to a destination due to a previously received DUPLICATE_WARNING
 * from one of its neighbors
 *
 * Test setup:                   | Description:
 *                               |   * We are testing from the perspective of (A)
 * (...)---(A)---(B)   (dest)    |   * A receives a DUPLICATE_WARNING packet from (B)
 *                               |   * first (A) deletes its only route do (dest) via (B)
 *                               |   * (A) has no other route so it broadcasts a ROUTE_FAILURE
 */
TEST(AbstractARAClientTest, brodcastRouteFailureIfAllAvailableroutesHaveBeenDeletedDueToDuplicateWarning) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("src"));
    AddressPtr nodeA = interface->getLocalAddress();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr destination (new AddressMock("dest"));

    // we are testing from the perspective of (A)
    routingTable->update(destination, nodeB, interface, 10);

    // start the test by receiving the DUPLICATE_WARNING packet from (B)
    Packet* packetThatCausedTheDuplicate = new Packet(source, destination, nodeA, PacketType::DATA, 123, 10);
    Packet* duplicateWarning = packetFactory->makeDuplicateWarningPacket(packetThatCausedTheDuplicate, nodeB, 1);
    client->receivePacket(duplicateWarning, interface);

    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    BYTES_EQUAL(1, sentPackets->size());

    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(interface->isBroadcastAddress(receiver));

    const Packet* lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(lastSentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(lastSentPacket->getDestination()->equals(destination));

    delete packetThatCausedTheDuplicate;
}

/**
 * This test checks if a client sends the ROUTE_FAILURE to a neighbor
 * if it has only one single next hop available for a specific destination.
 *
 * Test setup:                     | Description:
 *                                 |   * We are testing from the perspective of (A)
 * (...)---(A)-->(B)-(...)--(dest) |   * A receives a DUPLICATE_WARNING packet from (B)
 *          └--->(C)---------/     |   * first (A) deletes the route do (dest) via (B)
 *                                 |   * then is recognizes that the only remaining route leads via (C)
 *                                 |   * (A) send a ROUTE_FAILURE to (C) to prevent it from sending any traffic to (dest) via (A)
 */
TEST(AbstractARAClientTest, sendRouteFailureIfOnlyOneRouteIsLeftDueToDuplicateWarning) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("src"));
    AddressPtr nodeA = interface->getLocalAddress();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr destination (new AddressMock("destination"));

    // we are testing from the perspective of (A)
    routingTable->update(destination, nodeB, interface, 10);
    routingTable->update(destination, nodeC, interface, 10);

    // start the test by receiving the DUPLICATE_WARNING packet from (B)
    Packet* packetThatCausedTheDuplicate = new Packet(source, destination, nodeA, PacketType::DATA, 123, 10);
    Packet* duplicateWarning = packetFactory->makeDuplicateWarningPacket(packetThatCausedTheDuplicate, nodeB, 1);
    client->receivePacket(duplicateWarning, interface);

    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    CHECK_TRUE( routingTable->exists(destination, nodeC, interface));
    BYTES_EQUAL(1, sentPackets->size());

    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(receiver->equals(nodeC));

    const Packet* lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(lastSentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(lastSentPacket->getDestination()->equals(destination));

    delete packetThatCausedTheDuplicate;
}

/**
 * In this test we check that a client broadcasts a ROUTE_FAILURE to all of its neighbors
 * if it just deleted the last known route to a destination due to a recognized link failure
 *
 * Test setup:                   | Description:
 *                               |   * We are testing from the perspective of (A)
 * (...)---(A)---(B)   (dest)    |   * A recognizes that its link to (B) is broken
 *                               |   * first (A) deletes its only route do (dest) via (B)
 *                               |   * (A) has no other route so it broadcasts a ROUTE_FAILURE
 */
TEST(AbstractARAClientTest, brodcastRouteFailureIfAllAvailableroutesHaveBeenDeletedDueToBrokenLink) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("src"));
    AddressPtr nodeA = interface->getLocalAddress();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr destination (new AddressMock("dest"));

    // we are testing from the perspective of (A)
    routingTable->update(destination, nodeB, interface, 10);

    // start the test by recognizing the broken link to (B)
    Packet* somePacket = new Packet(source, destination, nodeA, PacketType::DATA, 123, 10);
    client->handleBrokenLink(somePacket, nodeB, interface);

    CHECK_FALSE(routingTable->exists(destination, nodeB, interface));
    BYTES_EQUAL(1, sentPackets->size());

    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(interface->isBroadcastAddress(receiver));

    const Packet* lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(lastSentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(lastSentPacket->getDestination()->equals(destination));
}

/**
 * In this test we check if a client deletes all known routes via one of its neighbors
 * if it recognized a link failure to it.
 *
 * Test setup:                   | Description:
 *                ┌--->(dest1)   |   * We are testing from the perspective of (A)
 * (...)---(A)---(B)-->(dest2)   |   * (A) knows that it can reach (dest1), (dest2) & (dest3) via (B)
 *          │     └--->(dest3)   |   * (A) recognizes a link failure to (B)
 *          │             │      |   * (A) should now remove a routing table entries which lead over (B)
 *          └--->(C)--->--┘      |   *     but it should not delete the remaining route via (C) since it is still valid
 */
TEST(AbstractARAClientTest, deleteAllKnownRoutesViaNextHopwhenLinkIsBroken) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr nodeA = interface->getLocalAddress();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr source (new AddressMock("source"));;
    AddressPtr dest1 (new AddressMock("dest1"));
    AddressPtr dest2 (new AddressMock("dest2"));
    AddressPtr dest3 (new AddressMock("dest3"));

    // we are testing from the perspective of (A)
    routingTable->update(dest1, nodeB, interface, 10);
    routingTable->update(dest2, nodeB, interface, 10);
    routingTable->update(dest3, nodeB, interface, 10);
    routingTable->update(dest3, nodeC, interface, 10);

    // start the test by recognizing the broken link to (B)
    Packet* somePacket = new Packet(source, dest1, nodeA, PacketType::DATA, 123, 10);
    client->handleBrokenLink(somePacket, nodeB, interface);

    // all routes over (B) should have been deleted
    CHECK_FALSE(routingTable->exists(dest1, nodeB, interface));
    CHECK_FALSE(routingTable->exists(dest2, nodeB, interface));
    CHECK_FALSE(routingTable->exists(dest3, nodeB, interface));

    // the route over (C) does still exist
    CHECK_TRUE(routingTable->exists(dest3, nodeC, interface));

    // the client should have broadcasted 2 ROUTE_FAILURE packets for (dest1) and (dest2)
    // additionally it should have sent 1 ROUTE_FAILURE unicast to (C) as this is the only remaining route to (dest3)
    BYTES_EQUAL(3, sentPackets->size());
    for (int i = 0; i < 3; ++i) {
        const Packet* sentPacket = sentPackets->at(i)->getLeft();
        AddressPtr receiver = sentPackets->at(i)->getRight();

        CHECK(sentPacket->getType() == PacketType::ROUTE_FAILURE);
        CHECK(interface->isBroadcastAddress(receiver) || receiver->equals(nodeC));
    }
}

/**
 * This test checks if a node sends a HELLO packet to all of its neighbors
 * which have not send or successfully received any packets lately.
 *
 * Test setup:                      | Description (Test from perspective of (A)):
 *          ┌--->(B)──(..)───┐      |   * After some time has passed the following happens:
 * (src)───(A)-->(C)──(..)──(dest1) |   * (A) has received a packet from (D)
 *          │└<─>(D)──(..)───┘│     |   * (A) has successfully send a packet via (E)
 *          └───>(E)──(..)────┘     |   * but (A) has not heard from (B) and (C) lately
 *                └─────────(dest2) |   * (A) should send a HELLO unicast to (B) and (C)
 *                                  |   * only (C) responds but for (B) a broken link is detected
 *                                  |   * the link to (C) should be deleted
 */
TEST(AbstractARAClientTest, sendHelloPacketToInactiveNeighbors) {
    activateNeighborActivityCheck(500);
    std::shared_ptr<TimerMock> neighborActivityTimer = std::dynamic_pointer_cast<TimerMock>((client->getNeighborActivityTimer()).lock());

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr nodeA = interface->getLocalAddress();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr nodeD (new AddressMock("D"));
    AddressPtr nodeE (new AddressMock("E"));
    AddressPtr source (new AddressMock("src"));;
    AddressPtr dest1 (new AddressMock("dest1"));
    AddressPtr dest2 (new AddressMock("dest2"));

    unsigned int seqNr = 1;

    // at first we create all routes via the reception some data from (dest)
    // create a route to relay the data
    routingTable->update(source, source, interface, 10);

    Packet* data1 = new Packet(dest1, source, nodeB, PacketType::DATA, seqNr++, 10);
    client->receivePacket(data1, interface);
    Packet* data2 = new Packet(dest1, source, nodeC, PacketType::DATA, seqNr++, 10);
    client->receivePacket(data2, interface);
    Packet* data3 = new Packet(dest1, source, nodeD, PacketType::DATA, seqNr++, 10);
    client->receivePacket(data3, interface);
    Packet* data4 = new Packet(dest1, source, nodeE, PacketType::DATA, seqNr++, 10);
    client->receivePacket(data4, interface);
    // we also have a route to dest2 via (E)
    Packet* data5 = new Packet(dest2, source, nodeE, PacketType::DATA, seqNr++, 10);
    client->receivePacket(data5, interface);

    // sanity check
    CHECK(routeIsKnown(dest1, nodeB, interface));
    CHECK(routeIsKnown(dest1, nodeC, interface));
    CHECK(routeIsKnown(dest1, nodeD, interface));
    CHECK(routeIsKnown(dest1, nodeE, interface));
    CHECK(routeIsKnown(dest2, nodeE, interface));

    // start the test by receiving a packet  from (D). This shows that (D) is still active and reachable
    TimeMock::letTimePass(300); // time is now at 300
    Packet* packetFromD = new Packet(dest1, source, nodeD, PacketType::DATA, seqNr++, 10);
    client->receivePacket(packetFromD, interface);

    // after some more time we successfully route a packet via (E). We know there is no error because layer 2 reports no problems.
    TimeMock::letTimePass(100); // time is now at 400
    Packet* packetviaE = new Packet(source, dest2, source, PacketType::DATA, seqNr++, 10);
    client->sendPacket(packetviaE);

    // the client should have relayed this packet via (E)
    // This is the seventh packet: 1-4. DATA from dest1, 5. DATA from dest2, 6. DATA from dest1, 7. DATA from src
    BYTES_EQUAL(7, sentPackets->size());
    const Packet* sentPacket = sentPackets->back()->getLeft();
    AddressPtr receiver = sentPackets->back()->getRight();
    CHECK(sentPacket->getSource()->equals(source) && sentPacket->getSequenceNumber() == seqNr-1);
    CHECK(receiver->equals(nodeE));

    // now even more time passes and the neighborActivityDelayInMs should be reached for (B) and (C)
    TimeMock::letTimePass(100); // time is now at 500
    neighborActivityTimer->expire();

    // the client should now try to reach (B) and (C) to see if they are still active
    BYTES_EQUAL(9, sentPackets->size());
    for (int i = 7; i <= 8; i++) {
        sentPacket = sentPackets->at(i)->getLeft();
        receiver = sentPackets->at(i)->getRight();

        CHECK(sentPacket->getType() == PacketType::HELLO);
        CHECK(sentPacket->getSource()->equals(nodeA));
        CHECK(sentPacket->getSender()->equals(nodeA));
        CHECK(sentPacket->getDestination()->equals(nodeB) || sentPacket->getDestination()->equals(nodeC));
    }

    // now simulate that (B) does not respond and there is a link brake detected at layer 2
    Packet* packetThatDetectedLinkBreak = new Packet(source, nodeB, nodeA, PacketType::HELLO, 5, 10);
    client->handleBrokenLink(packetThatDetectedLinkBreak, nodeB, interface);

    CHECK_FALSE(routeIsKnown(dest1, nodeB, interface));
    CHECK_TRUE(routeIsKnown(dest1, nodeC, interface));
    CHECK_TRUE(routeIsKnown(dest1, nodeD, interface));
    CHECK_TRUE(routeIsKnown(dest1, nodeE, interface));
    CHECK_TRUE(routeIsKnown(dest2, nodeE, interface));

    // we do now keep the other routes active and check that the client does not try to send
    // another HELLO to (B) since this route has already failed
    TimeMock::letTimePass(200); // time is now at 700
    Packet* keepAliveFromC = new Packet(dest1, source, nodeC, PacketType::DATA, seqNr++, 10);
    Packet* keepAliveFromD = new Packet(dest1, source, nodeD, PacketType::DATA, seqNr++, 10);
    Packet* keepAliveFromE = new Packet(dest1, source, nodeE, PacketType::DATA, seqNr++, 10);
    client->receivePacket(keepAliveFromC, interface);
    client->receivePacket(keepAliveFromD, interface);
    client->receivePacket(keepAliveFromE, interface);

    TimeMock::letTimePass(300); // time is now at 1000
    neighborActivityTimer->expire();

    // now it would have been time for a HELLO packet to (B) (last activity for that was the HELLO packet at 500)
    // we check that this has not happened an only 9 packets (6 + 3 data packets) have been send by the client
    BYTES_EQUAL(12, sentPackets->size());
}

/**
 * This simple test just checks that there is no exception when processing a HELLO packet.
 * The client does not need to take any actions because this packet has already been acknowledged on layer 2
 */
TEST(AbstractARAClientTest, clientCanHandleHelloPacket) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr neighbor (new AddressMock("neighbor"));;

    Packet* helloPacket = packetFactory->makeHelloPacket(neighbor, interface->getLocalAddress(), 123);
    client->receivePacket(helloPacket, interface);
}

/**
 * In this test we check the following scenario:
 *
 * Test setup:                       | Description
 *                                   |   * The link (C,dest) is broken
 * (src)──(A)──(B)──(C)--(dest)      |   * (C) sends a ROUTE_FAILURE because it has no more routes to (dest)
 *                                   |   * (B) receives this ROUTE_FAILURE, deletes the route via (C)
 *                                   |   * now (B) has no more routes and it needs to send a ROUTE_FAILURE itself
 *                                   |   * (A) does the same
 */
TEST(AbstractARAClientTest, chainRouteFailuresIfCriticalLinkBroke) {
    NetworkInterfaceMock* interfaceOfSrc = client->createNewNetworkInterfaceMock("src");

    ARAClientMock clientA = ARAClientMock();
    NetworkInterfaceMock* interfaceOfA = clientA.createNewNetworkInterfaceMock("A");
    SendPacketsList* sentPacketsOfA = interfaceOfA->getSentPackets();

    ARAClientMock clientB = ARAClientMock();
    NetworkInterfaceMock* interfaceOfB = clientB.createNewNetworkInterfaceMock("B");
    SendPacketsList* sentPacketsOfB = interfaceOfB->getSentPackets();

    ARAClientMock clientC = ARAClientMock();
    NetworkInterfaceMock* interfaceOfC = clientC.createNewNetworkInterfaceMock("C");
    SendPacketsList* sentPacketsOfC = interfaceOfC->getSentPackets();

    AddressPtr source = interfaceOfSrc->getLocalAddress();
    AddressPtr nodeA = interfaceOfA->getLocalAddress();
    AddressPtr nodeB = interfaceOfB->getLocalAddress();
    AddressPtr nodeC = interfaceOfC->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));

    routingTable->update(destination, nodeA, interfaceOfSrc, 10);
    clientA.getRoutingTable()->update(destination, nodeB, interfaceOfA, 10);
    clientB.getRoutingTable()->update(destination, nodeC, interfaceOfB, 10);
    clientC.getRoutingTable()->update(destination, destination, interfaceOfC, 10);

    // start the test by noticing the broken link
    Packet* somePacket = new Packet(source, destination, nodeC, PacketType::DATA, 123, 10);
    clientC.handleBrokenLink(somePacket, destination, interfaceOfC);

    // (C) should have broadcasted a ROUTE FAILURE
    BYTES_EQUAL(1, sentPacketsOfC->size());
    const Packet* routeFailurePacket = sentPacketsOfC->back()->getLeft();
    CHECK(routeFailurePacket->getType() == PacketType::ROUTE_FAILURE);

    // now let (B) receive this ROUTE_FAILURE
    clientB.handleBrokenLink(packetFactory->makeClone(routeFailurePacket), nodeC, interfaceOfB);

    // (B) should have broadcasted a ROUTE FAILURE
    BYTES_EQUAL(1, sentPacketsOfB->size());
    routeFailurePacket = sentPacketsOfB->back()->getLeft();
    CHECK(routeFailurePacket->getType() == PacketType::ROUTE_FAILURE);

    // now let (A) receive this ROUTE_FAILURE
    clientA.handleBrokenLink(packetFactory->makeClone(routeFailurePacket), nodeB, interfaceOfA);

    // (A) should have broadcasted a ROUTE FAILURE
    BYTES_EQUAL(1, sentPacketsOfA->size());
    routeFailurePacket = sentPacketsOfA->back()->getLeft();
    CHECK(routeFailurePacket->getType() == PacketType::ROUTE_FAILURE);
}

/**
 * In this test we check that the HELLO packet timers (neighbor activity timers) are only started for
 * arriving ANT or DATA packets.
 * Note: This concerns only the processing of received packets.
 */
TEST(AbstractARAClientTest, helloPacketTimersAreOnlystartedForDataAndAntPackets) {
    activateNeighborActivityCheck(500);
    std::shared_ptr<TimerMock> neighborActivityTimer = std::dynamic_pointer_cast<TimerMock>((client->getNeighborActivityTimer()).lock());
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr localAddress = interface->getLocalAddress();
    AddressPtr neighbor1 (new AddressMock("1"));
    AddressPtr neighbor2 (new AddressMock("2"));
    AddressPtr neighbor3 (new AddressMock("3"));
    AddressPtr neighbor4 (new AddressMock("4"));
    AddressPtr neighbor5 (new AddressMock("5"));
    AddressPtr neighbor6 (new AddressMock("6"));
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("dest"));

    unsigned int seqNr = 1;

    // start the test by receiving the packets

    // receiving a FANT should trigger a HELLO later
    Packet* fant = packetFactory->makeFANT(source, destination, seqNr++)->setSender(neighbor1);
    client->receivePacket(fant, interface);
    TimeMock::letTimePass(500);
    neighborActivityTimer->expire();
    BYTES_EQUAL(2, sentPackets->size());
    const Packet* lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::HELLO);
    client->forget(neighbor1);

    // receiving a BANT should trigger a HELLO later
    Packet* fantThatTriggeredBANT = packetFactory->makeFANT(source, destination, seqNr++);
    Packet* bant = packetFactory->makeBANT(fantThatTriggeredBANT, seqNr++)->setSender(neighbor2);
    client->receivePacket(bant, interface);
    TimeMock::letTimePass(500);
    neighborActivityTimer->expire();
    BYTES_EQUAL(4, sentPackets->size());
    lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::HELLO);
    client->forget(neighbor2);
    delete fantThatTriggeredBANT;

    // receiving a data packet should trigger a HELLO later
    Packet* dataPacket = packetFactory->makeDataPacket(source, destination, seqNr++, "Foo", 4)->setSender(neighbor3);
    client->receivePacket(dataPacket, interface);
    TimeMock::letTimePass(500);
    neighborActivityTimer->expire();
    BYTES_EQUAL(6, sentPackets->size());
    lastSentPacket = sentPackets->back()->getLeft();
    CHECK(lastSentPacket->getType() == PacketType::HELLO);
    client->forget(neighbor3);

    // receiving a DUPLICATE_WARNING should *not* trigger a HELLO later
    Packet packetThatCausedLoop = Packet(source, destination, localAddress, PacketType::DATA, seqNr++, 10);
    Packet* duplicateWarning = packetFactory->makeDuplicateWarningPacket(&packetThatCausedLoop, neighbor4, seqNr++);
    client->receivePacket(duplicateWarning, interface);
    TimeMock::letTimePass(500);
    neighborActivityTimer->expire();
    BYTES_EQUAL(6, sentPackets->size()); // still 6
    client->forget(neighbor4);

    // receiving a HELLO should *not* trigger another HELLO later
    Packet* helloPacket = packetFactory->makeHelloPacket(neighbor5, localAddress, seqNr++);
    client->receivePacket(helloPacket, interface);
    TimeMock::letTimePass(500);
    neighborActivityTimer->expire();
    BYTES_EQUAL(6, sentPackets->size()); // still 6
    client->forget(neighbor5);

    // receiving a ROUTE_FAILURE should *not* trigger a HELLO later
    Packet* routeFailurePacket = packetFactory->makeRouteFailurePacket(neighbor6, localAddress, seqNr++);
    client->receivePacket(routeFailurePacket, interface);
    TimeMock::letTimePass(500);
    neighborActivityTimer->expire();
    BYTES_EQUAL(6, sentPackets->size()); // still 6
    client->forget(neighbor5);
}

/**
 * In this test we check if a client is able to learn a new route if a previously collapsed route has died
 * and a new route discovery is run.
 *
 * Test setup:                       | Description
 *                                   |   * We test from the perspective of (A)
 * (src)──(A)──(B)──(...)--(dest)    |   * (A) receives FANT1 from src and broadcasts it further into the network
 *                                   |   * (A) receives BANT1 from (B) and creates a route to (dest) via (B)
 *                                   |   * (A) wants to send DATA1 and notices that the link to (B) is broken and broadcasts a ROUTE_FAILURE
 *                                   |   * (src) starts another route discovery
 *                                   |   * (A) receives FANT2 and broadcasts it into the network
 *                                   |   * (A) receives a BANT2 from (B) an creates a new route to (dest) via (B)
 */
TEST(AbstractARAClientTest, clientIsAbleToLearnNewRoutes) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    AddressPtr nodeA = interface->getLocalAddress();
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr source (new AddressMock("src"));
    AddressPtr destination (new AddressMock("dest"));

    Packet* fant1 = packetFactory->makeFANT(source, destination, 1);
    Packet* cloneOfFant1 = packetFactory->makeClone(fant1);
    Packet* bant1 = packetFactory->makeBANT(cloneOfFant1, 1)->setSender(nodeB);
    client->receivePacket(fant1, interface);
    client->receivePacket(bant1, interface);

    CHECK(routeIsKnown(destination, nodeB, interface));

    Packet* data1 = packetFactory->makeDataPacket(source, destination, 2, "Foo", 4)->setSender(nodeA);
    client->handleBrokenLink(data1, nodeB, interface);

    CHECK(routeIsKnown(destination, nodeB, interface) == false);

    Packet* fant2 = packetFactory->makeFANT(source, destination, 3);
    Packet* cloneOfFant2 = packetFactory->makeClone(fant2);
    Packet* bant2 = packetFactory->makeBANT(cloneOfFant2, 2)->setSender(nodeB);
    client->receivePacket(fant2, interface);
    client->receivePacket(bant2, interface);

    CHECK(routeIsKnown(destination, nodeB, interface));

    delete cloneOfFant1;
    delete cloneOfFant2;
}

/**
 * This test checks if the destructor does clean up the running pant timers.
 * The test succeeds if there is no memory leak at the end
 */
TEST(AbstractARAClientTest, scheduledPANTTimersAreDeletedInDestructor) {
    // at first we need our own configuration to enable this feature
    BasicConfiguration configuration = client->getStandardConfiguration();
    configuration.setPANTInterval(1000); // it doesn't really matter what value we set as long as this is > 0
    createNewClient(configuration);

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("dest");
    AddressPtr source (new AddressMock("src"));
    AddressPtr someNode (new AddressMock("A"));

    // we are testing from the perspective of (dest)
    routingTable->update(source, someNode, interface, 10);

    // start the test by letting (dest) receive a DATA packet
    Packet* somePacket = new Packet(source, interface->getLocalAddress(), someNode, PacketType::DATA, 1, 10, "Foo", 4);
    client->receivePacket(somePacket, interface);

    // check if the PANT timer is running
    CHECK(client->isPANTsTimerExpired(source) == false);
    CHECK(client->isPANTsTimerType(source, TimerType::PANTS_TIMER) == true);
    CHECK(client->isPANTsTimerRunning(source) == true);

    //TODO why do I need to clean this up (ARAClientMock should do this)
    delete somePacket;
}

/**
 * In this test we check that a client does only send a PANT if he is the destination of a transmission.
 */
TEST(AbstractARAClientTest, PANTSAreNotSendByIntermediateNodes) {
    // at first we need our own configuration to enable this feature
    BasicConfiguration configuration = client->getStandardConfiguration();
    configuration.setPANTInterval(1000); // it doesn't really matter what value we set as long as this is > 0
    createNewClient(configuration);

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("Foo");
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    AddressPtr someNode (new AddressMock("A"));

    // we are testing from the perspective of (src)
    routingTable->update(destination, someNode, interface, 10);

    // start the test by giving sending a packet to (destination)
    Packet* somePacket = new Packet(source, destination, source, PacketType::DATA, 1, 10, "Foo", 4);
    client->sendPacket(somePacket);

    // check that a PANT timer has *not* been started
    CHECK(client->isPANTsTimerRunning(destination) == false);
}

/**
 * In this test we check if a client who receives a PANT broadcasts over all of its interfaces
 */
TEST(AbstractARAClientTest, receivedPANTSPacketsAreBroadcasted) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock();
    SendPacketsList* sentPackets = interface->getSentPackets();

    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    Packet* pant = packetFactory->makePANT(source, destination, 123);
    unsigned int originalTTL = pant->getTTL();
    // start the test
    client->receivePacket(pant, interface);

    BYTES_EQUAL(1, sentPackets->size());
    const Packet* sentPacket = sentPackets->back()->getLeft();
    AddressPtr receiver = sentPackets->back()->getRight();

    // check the sent packet
    CHECK(interface->isBroadcastAddress(receiver));
    CHECK(sentPacket->getSource()->equals(source));
    CHECK(sentPacket->getDestination()->equals(destination));
    CHECK(sentPacket->getSender()->equals(interface->getLocalAddress()));
    CHECK_EQUAL(PacketType::PANT, sentPacket->getType());
    LONGS_EQUAL(originalTTL-1, sentPacket->getTTL());
}

/**
 * In this test we check that a client does only start to send out PANTS
 * to the directions he *received* data from
 */
TEST(AbstractARAClientTest, startToSendPacketsOnlyIfclientReceivesData) {
    // at first we need our own configuration to enable this feature
    BasicConfiguration configuration = client->getStandardConfiguration();
    configuration.setPANTInterval(1000); // it doesn't really matter what value we set as long as this is > 0 (timer is expired manually in the test)
    createNewClient(configuration);

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("destination");
    SendPacketsList* sentPackets = interface->getSentPackets();
    AddressPtr destination = interface->getLocalAddress();
    AddressPtr source1 (new AddressMock("src1"));
    AddressPtr source2 (new AddressMock("src2"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));

    // we are testing from the perspective of (destination)

    // start the test by receiving the first BANTs from (src1) and (src2)
    Packet* fant1 = new Packet(source1, destination, nodeA, PacketType::FANT, 1, 10);
    Packet* fant2 = new Packet(source2, destination, nodeB, PacketType::FANT, 1, 10);
    client->receivePacket(fant1, interface);
    client->receivePacket(fant2, interface);

    // the client should now have sent two BANTs back
    BYTES_EQUAL(2, sentPackets->size());
    CHECK(sentPackets->at(0)->getLeft()->getType() == PacketType::BANT);
    CHECK(sentPackets->at(1)->getLeft()->getType() == PacketType::BANT);

    // no PANTs should have been scheduled because those where only ANT packets (no DATA)
    CHECK(client->isPANTsTimerExpired(source1));

    // now we receive some DATA which should make us schedule PANT1
    Packet* dataPacket1 = new Packet(source1, destination, nodeA, PacketType::DATA, 2, 10, "Foo", 4);
    client->receivePacket(dataPacket1, interface);

    CHECK(client->isPANTsTimerExpired(source1) != true);
    CHECK(client->isPANTsTimerExpired(source2) == true);

    // check if PANT1 is sent
    CHECK(client->isPANTsTimerRunning(source1));
    TimeMock::letTimePass(1000);
    client->expirePANTsTimer(source1);

    BYTES_EQUAL(3, sentPackets->size());
    const Packet* sentPacket = sentPackets->back()->getLeft();
    CHECK(sentPacket->getType() == PacketType::PANT);
    CHECK(sentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(sentPacket->getDestination()->equals(source1));
    CHECK(sentPacket->getSender()->equals(interface->getLocalAddress()));
    BYTES_EQUAL(0, sentPacket->getPayloadLength());

    // now we receive traffic from (src2) which should make us send PANT2 (nut not PANT1 anymore since this has already been sent)
    Packet* dataPacket2 = new Packet(source2, destination, nodeB, PacketType::DATA, 2, 10, "Foo", 4);
    client->receivePacket(dataPacket2, interface);

    CHECK(client->isPANTsTimerExpired(source1) == true);
    CHECK(client->isPANTsTimerExpired(source2) != true);

    // check if PANT1 is sent
    CHECK(client->isPANTsTimerRunning(source2));
    TimeMock::letTimePass(1000);
    client->expirePANTsTimer(source2);

    BYTES_EQUAL(4, sentPackets->size());
    sentPacket = sentPackets->back()->getLeft();
    CHECK(sentPacket->getType() == PacketType::PANT);
    CHECK(sentPacket->getSource()->equals(interface->getLocalAddress()));
    CHECK(sentPacket->getDestination()->equals(source2));
    CHECK(sentPacket->getSender()->equals(interface->getLocalAddress()));
    BYTES_EQUAL(0, sentPacket->getPayloadLength());

    //TODO why do I need to clean this up (ARAClientMock should do this)
    delete dataPacket1;
    delete dataPacket2;
}

/**
 * In this test we disable the previous hop feature and check if the right routes are created.
 *
 * Test setup:                      | Description (Test from perspective of (C)):
 *          ┌───>(B)                |   * at first (C) gets a FANT from (A) so it learns this route
 *          │     │                 |   * then it gets the same FANT from (B) and also creates the route
 * (src)───(A)──>(C)──(..)──(dest)  |     no matter if it already knows the previous hop
 */
TEST(AbstractARAClientTest, previousHopFeatureCanBeDisabled) {
    // at first we need our own configuration to enable this feature
    BasicConfiguration configuration = client->getStandardConfiguration();
    configuration.deactivatePreviousHopFeature();
    createNewClient(configuration);

    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("C");
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC = interface->getLocalAddress();
    AddressPtr source (new AddressMock("src"));
    AddressPtr destination (new AddressMock("dest"));

    // we test from the perspective of node C
    CHECK_FALSE(routingTable->isDeliverable(source));

    // at first we receive the broadcast from A
    Packet* fantFromA = new Packet(source, destination, nodeA, PacketType::FANT, 1, 10);
    client->receivePacket(fantFromA, interface);

    // this should have created a route to (src) via (A)
    CHECK_TRUE(routeIsKnown(source, nodeA, interface));

    // now we receive the broadcast from (B)
    Packet* fantFromB = new Packet(source, destination, nodeB, PacketType::FANT, 1, 9);
    // even if we would set the previousHop to (A) the client would ignore this and still create the route
    fantFromB->setPreviousHop(nodeA);
    client->receivePacket(fantFromB, interface);

    // this should also create the route to (src) via (B)
    CHECK_TRUE(routeIsKnown(source, nodeB, interface));
}
