/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "TestMacros.h"
#include "PacketTrap.h"
#include "EnergyAwareRoutingTable.h"
#include "EARAPacketFactory.h"
#include "EARAPacket.h"

#include "testAPI/mocks/EARAClientMock.h"
#include <iostream>

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

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
    fant1->decreaseTTL(4); // traveled 4 hops
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
    fant2->decreaseTTL(4); // traveled 4 hops
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
    fant3->decreaseTTL(4); // traveled 4 hops
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
    fant4->decreaseTTL(4); // traveled 4 hops
    client->receivePacket(fant4, interface);
    DOUBLES_EQUAL(0.4, routingTable->getEnergyValue(source, nextHop, interface), 0.0001);
}
