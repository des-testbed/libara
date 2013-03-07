#include "CppUTest/TestHarness.h"
#include "OMNeTClock.h"
#include "Time.h"
#include "OMNeTTime.h"

using namespace ARA;
using namespace ARA::omnetpp;

TEST_GROUP(OMNeTClockTest) {};

TEST(OMNeTClockTest, makeTime) {
    OMNeTClock clock = OMNeTClock();
    Time* newTime = clock.makeTime();

    OMNeTTime* omnetTimeInstance = dynamic_cast<OMNeTTime*>(newTime);
    CHECK(omnetTimeInstance != NULL);

    delete newTime;
}
