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
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/LoggerMock.h"
#include "testAPI/mocks/time/ClockMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(AbstractARAClientTest) {
    ARAClientMock* client;
    PacketTrap* packetTrap;
    RoutingTable* routingTable;
    PacketFactory* packetFactory;

    void setup() {
        client = new ARAClientMock();
        packetTrap = client->getPacketTrap();
        routingTable = client->getRoutingTable();
        packetFactory = Environment::getPacketFactory();
    }

    void teardown() {
        delete client;
    }

    /**
     * Returns true iff a route to destination via nextHop and interface is known in the RoutingTable.
     */
    bool routeIsKnown(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
        std::deque<RoutingTableEntry*>* possibleNextHops = routingTable->getPossibleNextHops(destination);
        if(possibleNextHops->empty()) {
            delete possibleNextHops;
            return false;
        }
        else {
            for(auto& possibleHop: *possibleNextHops) {
                if(possibleHop->getAddress()->equals(nextHop) && possibleHop->getNetworkInterface()->equals(interface)) {
                    return true;
                }
            }
            return false;
        }
    }
};

TEST(AbstractARAClientTest, packetGetsTrappedIfNotDeliverable) {
    Packet* packet = new PacketMock();

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
    Packet* packet = new PacketMock();

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
    unsigned int originalHopCount = 5;
    unsigned int originalSequenceNr = 123;
    const char* originalPayload = "Hello";
    int originalPayloadSize = std::strlen(originalPayload) + 1;
    Packet* packet = new Packet(originalSource, originalDestination, originalSender, PacketType::DATA, originalSequenceNr, originalPayload, originalPayloadSize, originalHopCount);

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

    // only the hop count needs to be incremented by 1
    CHECK_EQUAL(originalHopCount + 1, sentPacket->getHopCount());
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
 * In this test we simulate that the same data packet has been received
 * twice at node x. The packet is directed from node A to node B and
 * has been relayed via node C to node X.
 * Node x must respond to node C with a DUPLICATE_ERROR packet.
 */
TEST(AbstractARAClientTest, respondWithDuplicateError) {
    // prepare a packet
    AddressPtr source (new AddressMock("A"));
    AddressPtr destination (new AddressMock("B"));
    AddressPtr sender (new AddressMock("C"));
    Packet* firstPacket = new Packet(source, destination, sender, PacketType::DATA, 123, 1);
    Packet* secondPacket = new Packet(source, destination, sender, PacketType::DATA, 123, 1);

    // let client receive the packet over the same interface twice
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("X");
    client->receivePacket(firstPacket, interface);
    client->receivePacket(secondPacket, interface);

    // the client should have relayed the first packet and sent a duplicate warning back for the second packet
    LONGS_EQUAL(2, interface->getNumberOfSentPackets());

    Pair<const Packet*, AddressPtr>* sentPacketInfo = interface->getSentPackets()->at(1); // we only check the warning
    const Packet* sentPacket = sentPacketInfo->getLeft();
    AddressPtr recipientOfSentPacket = sentPacketInfo->getRight();

    // check the contents of the duplicate warning packet
    CHECK(recipientOfSentPacket->equals(sender));
    CHECK(sentPacket->getSender()->equals(interface->getLocalAddress()));
    CHECK(sentPacket->getSource()->equals(source));
    CHECK(sentPacket->getType() == PacketType::DUPLICATE_ERROR);
    LONGS_EQUAL(1, sentPacket->getHopCount());
    CHECK_EQUAL(0, sentPacket->getPayloadLength());
}

/**
 * In this test we simulate that a FANT or a BANT have been received
 * twice at node x. This kind of duplication must simply be ignored
 * by node x (no further broadcasting of the ant packet).
 */
TEST(AbstractARAClientTest, ignoreDuplicateAntPackets) {
    // prepare the ant packets
    Packet* fant1 = new PacketMock("A", "B", "C", 123, 1, PacketType::FANT);
    Packet* fant2 = new PacketMock("A", "B", "C", 123, 1, PacketType::FANT);
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

    LONGS_EQUAL(2, interface->getNumberOfSentPackets()); // only two broadcasts of the ANT packets and nothing more
    std::deque<Pair<const Packet*, AddressPtr>*>* sendPackets = interface->getSentPackets();
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
    Packet* packet = new Packet(nodeA, nodeB, nodeC, PacketType::DATA, 123);

    CHECK(routingTable->isDeliverable(nodeA) == false);
    client->receivePacket(packet, interface);
    CHECK(routingTable->isDeliverable(nodeA) == true);
    std::deque<RoutingTableEntry*>* possibleHops = routingTable->getPossibleNextHops(nodeA);
    LONGS_EQUAL(1, possibleHops->size());
    CHECK(possibleHops->front()->getAddress()->equals(nodeC));
    CHECK(possibleHops->front()->getNetworkInterface()->equals(interface));
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
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();

    AddressPtr source (new AddressMock("B"));
    AddressPtr destination (new AddressMock("C"));
    AddressPtr sender = source;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, 123, "Hello World");
    packet->increaseHopCount();

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
    LONGS_EQUAL(2, sentPacket->getHopCount());
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
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();

    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    Packet* fantPacket = new Packet(nodeB, nodeC, nodeB, PacketType::FANT, 123, 1);
    Packet* bantPacket = new Packet(nodeC, nodeB, nodeC, PacketType::BANT, 345, 1);

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
    LONGS_EQUAL(2, sentPacket1->getHopCount());

    // check the first sent packet
    const Packet* sentPacket2 = sentPacketInfo2->getLeft();
    CHECK(sentPacket2->getSource()->equals(nodeC));
    CHECK(sentPacket2->getDestination()->equals(nodeB));
    CHECK(sentPacket2->getSender()->equals(interface->getLocalAddress()));
    CHECK_EQUAL(PacketType::BANT, sentPacket2->getType());
    LONGS_EQUAL(2, sentPacket2->getHopCount());
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
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();
    unsigned int lastSequenceNumber = client->getNextSequenceNumber();

    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeC (new AddressMock("C"));
    Packet* packet = new Packet(nodeB, nodeA, nodeC, PacketType::FANT, 123);

    // start the test
    client->receivePacket(packet, interface);

    // check the sent packet
    CHECK(sentPackets->size() == 1);
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(interface->isBroadcastAddress(sentPacketInfo->getRight()));
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getSource()->equals(nodeA));
    CHECK(sentPacket->getDestination()->equals(nodeB));
    CHECK(sentPacket->getSender()->equals(nodeA));
    CHECK_EQUAL(PacketType::BANT, sentPacket->getType());
    LONGS_EQUAL(1, sentPacket->getHopCount());
    LONGS_EQUAL(lastSequenceNumber+1, sentPacket->getSequenceNumber());
}

/**
 * In this test node A has initiated a route discovery for a trapped data packet
 * Now it has received a BANT that has come back from the data packets destination.
 * It is expected that the trapped packet is send over the discovered route and is
 * no longer trapped in the PacketTrap.
 */
TEST(AbstractARAClientTest, receivedBANTTriggersSendingOfTrappedPackets) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();

    AddressPtr source (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr destination (new AddressMock("C"));
    AddressPtr sender = source;
    Packet* dataPacket = new Packet(source, destination, sender, PacketType::DATA, 123, "Hello World");
    packetTrap->trapPacket(dataPacket);

    Packet* bant = new Packet(destination, source, nodeB, PacketType::BANT, 123, 2);

    // start the test
    client->receivePacket(bant, interface);

    // check the sent packet
    LONGS_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(sentPacketInfo->getRight()->equals(nodeB));
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(sentPacket->getSource()->equals(source));
    CHECK(sentPacket->getDestination()->equals(destination));
    CHECK(sentPacket->getSender()->equals(interface->getLocalAddress()));
    CHECK_EQUAL(PacketType::DATA, sentPacket->getType());
    LONGS_EQUAL(1, sentPacket->getHopCount());

    // packet trap must now be empty
    CHECK(packetTrap->isEmpty());
}

/**
 * In this test we check if a client broadcasts a FANT again that he has
 * initially created and sent in the first place.
 */
TEST(AbstractARAClientTest, doNotReBroadcastFANT) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();
    Packet* data = new PacketMock("A", "Z", "A", 123, 1, PacketType::DATA);

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
    answer->increaseHopCount();
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
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();
    Packet* fant = new PacketMock("A", "Z", "A", 123, 1, PacketType::FANT);

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
    answer->increaseHopCount();
    answer->setSender(neighborAddress);
    client->receivePacket(answer, interface);

    // the client should not broadcast the BANT again
    CHECK(sentPackets->empty())

    // delete the sent packet (destructor does not know about it because we cleared the list of sent packets earlier)
    delete sentPacket;
}

/**
 * In this test we check if a client deletes the routing table entry to
 * another note if he receives a duplicate warning from this node.
 *
 * Test setup
 * Before test:        After test:
 *
 *    /--> (B)            /--> (B)
 *  (A)--> (C)          (A)
 *    \--> (D)            \--> (D)
 */
TEST(AbstractARAClientTest, receiveDuplicateErrorPacket) {
    // initial test setup
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");

    PacketMock duplicateErrorPacket = PacketMock("A", "X", "C", 123, 1, PacketType::DUPLICATE_ERROR);

    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr nodeC (new AddressMock("C"));
    AddressPtr nodeD (new AddressMock("D"));
    AddressPtr destination (new AddressMock("X"));
    routingTable->update(destination, nodeB, interface, 1.1);
    routingTable->update(destination, nodeC, interface, 2.2);
    routingTable->update(destination, nodeD, interface, 3.3);

    // Check some basic assumptions about this test setup
    CHECK(routeIsKnown(destination, nodeB, interface));
    CHECK(routeIsKnown(destination, nodeC, interface));
    CHECK(routeIsKnown(destination, nodeD, interface));

    // start the test
    client->receivePacket(&duplicateErrorPacket, interface);
    CHECK(routeIsKnown(destination, nodeB, interface));
    CHECK(routeIsKnown(destination, nodeC, interface) == false);
    CHECK(routeIsKnown(destination, nodeD, interface));
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
    Packet* dataPacket = new PacketMock("source", "destination", 123, 1, PacketType::DATA);

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
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();
    Packet* packet = new PacketMock();

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
    Packet* packetToOtherDestiantion = new PacketMock("source", "otherDestination", 4);
    Packet* packet1 = new PacketMock("source", "destination", 1);
    Packet* packet2 = new PacketMock("source", "destination", 2);
    Packet* packet3 = new PacketMock("source", "destination", 3);

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
    AddressPtr source (new AddressMock("source"));
    AddressPtr destination (new AddressMock("destination"));
    Packet* fant = new Packet(source, source, destination, PacketType::FANT, 123);

    // sanity check
    CHECK(routingTable->isDeliverable(source) == false);

    // start the test
    client->receivePacket(fant, interface);

    // we should still not know this address in the routing table
    CHECK(routingTable->isDeliverable(source) == false);
}

TEST(AbstractARAClientTest, pathToSourceIsReinforced) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("C");
    AddressPtr source(new AddressMock("A"));
    AddressPtr sender(new AddressMock("B"));
    AddressPtr destination(new AddressMock("D"));

    CHECK(!(routingTable->exists(source, sender, interface)));
    Packet* fant = new Packet(source, destination, sender, PacketType::FANT, 123, 1);
    client->receivePacket(fant, interface);
    CHECK(routingTable->exists(source, sender, interface));
    float currentPhi = routingTable->getPheromoneValue(source, sender, interface);
    Packet* data = new Packet(destination, source, sender, PacketType::DATA, 124, 1);
    client->receivePacket(data, interface);
    float newPhi = routingTable->getPheromoneValue(source, sender, interface);
    CHECK(newPhi > currentPhi);
}

TEST(AbstractARAClientTest, pathToDestinationIsReinforced) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("X");
    AddressPtr source(new AddressMock("A"));
    AddressPtr sender(new AddressMock("B"));
    AddressPtr destination(new AddressMock("C"));

    CHECK(!(routingTable->exists(destination, sender, interface)));
    Packet* bant = new Packet(destination, source, sender, PacketType::BANT, 123, 1);
    client->receivePacket(bant, interface);
    CHECK(routingTable->exists(destination, sender, interface));
    float currentPhi = routingTable->getPheromoneValue(destination, sender, interface);
    Packet* data = new Packet(source, destination, sender, PacketType::DATA, 124, 1);
    client->receivePacket(data, interface);
    float newPhi = routingTable->getPheromoneValue(destination, sender, interface);
    CHECK(newPhi > currentPhi);
}

TEST(AbstractARAClientTest, pathToDestinationEvaporates) {
    NetworkInterface* interface = client->createNewNetworkInterfaceMock("B");
    AddressPtr source(new AddressMock("source"));
    AddressPtr destination(new AddressMock("destination"));
    AddressPtr nodeA(new AddressMock("A"));
    AddressPtr nodeC(new AddressMock("C"));

    // sanity check
    CHECK(routeIsKnown(source, nodeA, interface) == false);
    CHECK(routeIsKnown(destination, nodeC, interface) == false);

    // send FANT (source --> A --> _B_ --> C --> destination)
    Packet* fant = new Packet(source, destination, nodeA, PacketType::FANT, 123, 1);
    client->receivePacket(fant, interface);
    CHECK(routeIsKnown(source, nodeA, interface));

    // send BANT (source <-- A <-- _B_ <-- C <-- destination)
    Packet* bant = new Packet(destination, source, nodeC, PacketType::BANT, 124, 1);
    client->receivePacket(bant, interface);
    CHECK(routeIsKnown(destination, nodeC, interface));

    float oldPhiToDest = routingTable->getPheromoneValue(destination, nodeC, interface);
    float oldPhiToSrc = routingTable->getPheromoneValue(source, nodeA, interface);

    // send DATA (source --> A --> _B_ --> C --> destination)
    CHECK(routingTable->isNewRoute(source, nodeA, interface) == false);
    Packet* data = new Packet(source, destination, nodeA, PacketType::DATA, 125, 1);
    client->receivePacket(data, interface);

    // check if the reinforcement has worked on both sides of the route
    float newPhiToDest = routingTable->getPheromoneValue(destination, nodeC, interface);
    float newPhiToSrc = routingTable->getPheromoneValue(source, nodeA, interface);

    CHECK(newPhiToDest > oldPhiToDest);
    CHECK(newPhiToSrc > oldPhiToSrc);

    // update the values so we can check the evaporation
    oldPhiToDest = newPhiToDest;
    oldPhiToSrc = newPhiToSrc;

    TimeMock::letTimePass(2000);

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
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender = source;
    AddressPtr destination (new AddressMock("destination"));
    unsigned int originalSeqNr = 123;
    unsigned int originalHopCount = 5;
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, originalSeqNr, "Foo", 4, originalHopCount);
    AddressPtr route1 (new AddressMock("route1"));
    AddressPtr route2 (new AddressMock("route2"));

    // create two known routes to the destination
    routingTable->update(destination, route1, interface, 20);
    routingTable->update(destination, route2, interface, 10);

    // sanity check
    CHECK(routingTable->isDeliverable(destination));
    CHECK(sentPackets->empty());

    // start the test
    client->handleRouteFailure(packet, route1, interface);

    // the client is expected to delete the route from the routing table
    CHECK(routingTable->exists(destination, route1, interface) == false);

    // the packet should have been sent via the other route
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    CHECK(sentPacketInfo->getRight()->equals(route2));

    CHECK_PACKET(sentPacketInfo->getLeft(), PacketType::DATA, originalSeqNr, source, sender, destination, originalHopCount, "Foo");
}

TEST(AbstractARAClientTest, broadcastRouteFailureIfNoAlternativeRouteAreKownOnRouteFailure) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("sender");
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender = interface->getLocalAddress();
    AddressPtr destination (new AddressMock("destination"));
    Packet* packet = new Packet(source, destination, sender, PacketType::DATA, 123);
    AddressPtr nextHop (new AddressMock("nextHop"));

    // create a known route to the destination
    routingTable->update(destination, nextHop, interface, 10);

    // sanity check
    CHECK(routingTable->isDeliverable(destination));
    CHECK(sentPackets->empty());

    // start the test
    client->handleRouteFailure(packet, nextHop, interface);

    // the client is expected to delete the route from the routing table
    CHECK(routingTable->exists(destination, nextHop, interface) == false);

    // the client should have sent a broadcast indicating that he can no longer relay to that destination
    BYTES_EQUAL(1, sentPackets->size());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = sentPackets->front();
    const Packet* sentPacket = sentPacketInfo->getLeft();
    CHECK(interface->isBroadcastAddress(sentPacketInfo->getRight()));
    CHECK(sentPacket->getType() == PacketType::ROUTE_FAILURE);
    CHECK(sentPacket->getSource()->equals(source));
    CHECK(sentPacket->getDestination()->equals(destination));
}

TEST(AbstractARAClientTest, clientsDeleteRoutingTableEntryWhenTheyReceiveRoutingFailurePacket) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("sender");
    AddressPtr source (new AddressMock("source"));
    AddressPtr sender (new AddressMock("sender"));
    AddressPtr destination (new AddressMock("destination"));
    Packet* routeFailurePacket = new Packet(source, destination, sender, PacketType::ROUTE_FAILURE, 123);

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
    std::deque<Pair<const Packet*, AddressPtr>*>* sentPackets = interface->getSentPackets();

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
 *        /--(A)--\
 * (source)       (testNode)--(C)
 *        \--(B)--/
 *
 */
TEST(AbstractARAClientTest, rememberMultipleRoutes) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("testNode");
    AddressPtr source (new AddressMock("source"));
    AddressPtr nodeA (new AddressMock("A"));
    AddressPtr nodeB (new AddressMock("B"));
    AddressPtr destination (new AddressMock("destination"));
    Packet* packetFromA = new Packet(source, destination, nodeA, PacketType::FANT, 123);
    Packet* packetFromB = packetFactory->makeClone(packetFromA);
    packetFromB->setSender(nodeB);

    // sanity check
    CHECK_FALSE(routeIsKnown(source, nodeA, interface));
    CHECK_FALSE(routeIsKnown(source, nodeB, interface));

    // start the test
    client->receivePacket(packetFromA, interface);
    CHECK_TRUE(routeIsKnown(source, nodeA, interface));
    CHECK_FALSE(routeIsKnown(source, nodeB, interface));

    client->receivePacket(packetFromB, interface);
    CHECK_TRUE(routeIsKnown(source, nodeA, interface));
    CHECK_TRUE(routeIsKnown(source, nodeB, interface));
}
