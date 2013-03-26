/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "Environment.h"
#include "Clock.h"
#include "PacketFactory.h"
#include "testAPI/mocks/time/ClockMock.h"
#include "testAPI/mocks/PacketMock.h"

using namespace ARA;

TEST_GROUP(EnvironmentTest) {};

/**
 * We test if we can retrieve some instance of Time and make sure it can be used.
 */
TEST(EnvironmentTest, getClock) {
    Clock* clock = Environment::getClock();
    Time* time = clock->makeTime();
    time->setToCurrentTime();
    delete time;
}

/**
 * We test if we can retrieve some instance of PacketFactory and make sure it can be used.
 */
TEST(EnvironmentTest, getPacketFactory) {
    PacketFactory* factory = Environment::getPacketFactory();
    PacketMock somePacket = PacketMock();
    Packet* anotherPacket = factory->makeFANT(&somePacket, 345);
    delete anotherPacket;
}
