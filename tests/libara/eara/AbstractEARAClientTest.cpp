/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "TestMacros.h"
#include "PacketTrap.h"
#include "EnergyAwareRoutingTable.h"
#include "Environment.h"
#include "Pair.h"
#include "PacketType.h"

#include "testAPI/mocks/EARAClientMock.h"
#include "testAPI/mocks/time/TimerMock.h"

#include <deque>

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(AbstractEARAClientTest) {
    EARAClientMock* client;
    PacketTrap* packetTrap;
    EnergyAwareRoutingTable* routingTable;
    PacketFactory* packetFactory;

    void setup() {
        client = new EARAClientMock();
        packetTrap = client->getPacketTrap();
        routingTable = client->getRoutingTable();
        packetFactory = client->getPacketFactory();
    }

    void teardown() {
        delete client;
    }
};

TEST(AbstractEARAClientTest, energyDisseminationTimerIsstartedAgain) {
    // prepare the test
    TimerMock* timer = client->getEnergyDisseminationTimer();

    // start the test
    CHECK(timer->isRunning());

    // let the timer expire
    timer->expire();

    // the timer should have been restarted after the timeout has been processed
    CHECK(timer->isRunning());
}
