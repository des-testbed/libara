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

    EARAPacket* getEARAPacket(const Packet* packet) {
        EARAPacket* earaPacket = dynamic_cast<EARAPacket*>(const_cast<Packet*>(packet));
        if (earaPacket == NULL) {
            stringstream errorMessage;
            errorMessage << PacketType::getAsString(packet->getType()) << " packet with seqNr " << packet->getSequenceNumber() << " is no EARAPacket!" << endl;
            FAIL(errorMessage.str().c_str());
        }

        return earaPacket;
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

    EARAPacket* fant = getEARAPacket(packetFactory->makeFANT(source, destination, 123));
    fant->addEnergyValue(60);

    // sanity check
    BYTES_EQUAL(60, fant->getMinimumEnergyValue());
    BYTES_EQUAL(60, fant->getTotalEnergyValue());

    // start the test
    client->receivePacket(fant, interface);

    // packet should have been updated and broadcasted
    LONGS_EQUAL(1, interface->getNumberOfSentPackets());
    Pair<const Packet*, AddressPtr>* sentPacketInfo = interface->getSentPackets()->front();
    EARAPacket* sentPacket = getEARAPacket(sentPacketInfo->getLeft());
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
 */
TEST(AbstractEARAClientTest, initializeEnergyValues) {

}
