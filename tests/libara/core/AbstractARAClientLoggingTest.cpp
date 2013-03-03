/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "PacketType.h"
#include "Address.h"
#include "Pair.h"
#include "Logger.h"

#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/LoggerMock.h"

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(AbstractARAClientLoggerTest) {
    ARAClientMock* client;
    PacketTrap* packetTrap;
    RoutingTable* routingTable;
    LoggerMock* logger;

    void setup() {
        client = new ARAClientMock();
        packetTrap = client->getPacketTrap();
        routingTable = client->getRoutingTable();

        logger = new LoggerMock();
        client->setLogger(logger);
    }

    void teardown() {
        delete client;
    }

    /**
     * Checks if one of the logged messages equals the given text and log level
     */
    bool hasLoggedMessage(string message, Logger::Level level) {
        deque<LogMessage>* loggedMessages = logger->getLoggedMessages();
        for(auto& loggedMessage: *loggedMessages) {
            if(loggedMessage.level == level && loggedMessage.text == message) {
                return true;
            }
        }

        return false;
    }
};

TEST(AbstractARAClientLoggerTest, sendsLogMessageIfAPacketIsTrappedAndFANTIsBroadcasted) {
    Packet* packet = new PacketMock("abc", "xyz", 123);

    // assume that the packet is not deliverable
    CHECK(routingTable->isDeliverable(packet) == false);
    client->sendPacket(packet);

    // check that the log message is generated
    LONGS_EQUAL(1, logger->getNrOfLoggedMessages());
    CHECK(hasLoggedMessage("Packet 123 from abc to xyz is not deliverable. Starting route discovery phase", Logger::LEVEL_DEBUG));
}

TEST(AbstractARAClientLoggerTest, sendsLogMessageIfFANTReachedItsDestination) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("destination");
    PacketMock packet = PacketMock("source", "destination", 123, 10, PacketType::FANT);

    client->receivePacket(&packet, interface);

    // check that the log message is generated
    LONGS_EQUAL(1, logger->getNrOfLoggedMessages());
    CHECK(hasLoggedMessage("FANT 123 from source reached its destination. Broadcasting BANT", Logger::LEVEL_DEBUG));
}

TEST(AbstractARAClientLoggerTest, sendsLogMessageIfBANTReachedItsDestination) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("source");
    PacketMock packet = PacketMock("destination", "source", 123, 10, PacketType::BANT);
    PacketMock trappedPacket = PacketMock("source", "destination", 1, 1, PacketType::DATA);
    packetTrap->trapPacket(&trappedPacket);

    client->receivePacket(&packet, interface);

    // check that the log message is generated
    CHECK(hasLoggedMessage("BANT 123 came back from destination. 1 trapped packet can now be delivered", Logger::LEVEL_DEBUG));
}

TEST(AbstractARAClientLoggerTest, sendsLogMessageIfAntPacketIsBroadcasted) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("A");
    PacketMock antPacket = PacketMock("source", "destination", 123, 3, PacketType::FANT);
    client->receivePacket(&antPacket, interface);

    // check that the log message is generated
    LONGS_EQUAL(1, logger->getNrOfLoggedMessages());
    CHECK(hasLoggedMessage("Broadcasting FANT 123 from source", Logger::LEVEL_TRACE));
}

TEST(AbstractARAClientLoggerTest, sendsLogMessageIfDataPacketIsRelayed) {
    NetworkInterfaceMock* interface = client->createNewNetworkInterfaceMock("192.168.0.2");
    PacketMock dataPacket = PacketMock("192.168.0.1", "192.168.0.10", 123, 4, PacketType::DATA);

    // create a route for this packet
    AddressPtr nextHop (new AddressMock("192.168.0.3"));
    routingTable->update(dataPacket.getDestination(), nextHop, interface, 10);

    CHECK(routingTable->isDeliverable(&dataPacket));
    client->receivePacket(&dataPacket, interface);

    // check that the log message is generated
    LONGS_EQUAL(1, logger->getNrOfLoggedMessages());
    CHECK(hasLoggedMessage("Forwarding DATA packet 123 from 192.168.0.1 to 192.168.0.10 via 192.168.0.3", Logger::LEVEL_TRACE));
}
