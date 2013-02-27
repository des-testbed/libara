#include "CppUTest/TestHarness.h"
#include "OMNeTTimeFactory.h"
#include "Time.h"
#include "OMNeTTime.h"

using namespace ARA;
using namespace ARA::omnetpp;

TEST_GROUP(OMNeTTimeFactory) {};

TEST(OMNeTTimeFactory, makeTime) {
    OMNeTTimeFactory timeFactory = OMNeTTimeFactory();
    Time* newTime = timeFactory.makeTime();

    OMNeTTime* timeMockInstance = dynamic_cast<OMNeTTime*>(newTime);
    CHECK(timeMockInstance != NULL);

    delete newTime;
}
