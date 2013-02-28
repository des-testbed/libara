#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/TimeFactoryMock.h"
#include "testAPI/mocks/TimeMock.h"
#include "Time.h"

using namespace ARA;

TEST_GROUP(TimeFactoryMockTest) {};

TEST(TimeFactoryMockTest, makeTime) {
    TimeFactoryMock timeFactory = TimeFactoryMock();
    Time* newTime = timeFactory.makeTime();

    TimeMock* timeMockInstance = dynamic_cast<TimeMock*>(newTime);
    CHECK(timeMockInstance != NULL);

    delete newTime;
}
