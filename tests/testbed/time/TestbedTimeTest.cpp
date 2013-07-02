/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"

#include "TestbedTime.h"

using namespace ARA::testbed;

TEST_GROUP(TestbedTimeTest) {
    std::chrono::time_point<std::chrono::system_clock> timestampA;
    std::chrono::time_point<std::chrono::system_clock> timestampB;
    void setup() {
        // we just get the current time
        time_t rawtime;
        time(&rawtime);
        // pass it to our own timeval data structure
        struct tm* timeinfo;
        timeinfo = localtime(&rawtime);
        // customize the fields
        timeinfo->tm_year = 2013 - 1900;
        timeinfo->tm_mon = 7 - 1;
        timeinfo->tm_mday = 2;
        timeinfo->tm_sec = 2;
        timeinfo->tm_min = 2;
        timeinfo->tm_hour = 15;
        
        time_t timeA = mktime(timeinfo);
        timestampA = std::chrono::system_clock::from_time_t(timeA);

        timeinfo->tm_mday = 3;
		time_t timeB = mktime(timeinfo);
        timestampB = std::chrono::system_clock::from_time_t(timeB);
    }
};

TEST(TestbedTimeTest, create) {
    TestbedTime time = TestbedTime(timestampA);
    LONGS_EQUAL(1372770122, time.getSeconds());
    LONGS_EQUAL(1372770122000, time.getMilliSeconds());
}

TEST(TestbedTimeTest, equality) {
    TestbedTime originalTime = TestbedTime(timestampA);
    TestbedTime anotherTime = TestbedTime(timestampB);
    TestbedTime sameTime = TestbedTime(timestampA);

    CHECK_FALSE(originalTime == anotherTime);
    CHECK_TRUE(originalTime == originalTime);
    CHECK_TRUE(originalTime == sameTime);
    CHECK_TRUE(sameTime == originalTime);
}

TEST(TestbedTimeTest, copyConstructor) {
    TestbedTime originalTime = TestbedTime(timestampA);
    TestbedTime timeCopy = TestbedTime(originalTime);

    CHECK(originalTime == timeCopy);
}
TEST(TestbedTimeTest, getSeconds) {
    TestbedTime time = TestbedTime(timestampA);
    LONGS_EQUAL(1372770122, time.getSeconds());
}

TEST(TestbedTimeTest, getMilliSeconds) {
    TestbedTime time = TestbedTime(timestampA);
    LONGS_EQUAL(1372770122000, time.getMilliSeconds());
}

TEST(TestbedTimeTest, getDifferenceInMilliSeconds) {
    TestbedTime timeA = TestbedTime(timestampA);
    TestbedTime timeB = TestbedTime(timestampB);
    /// FIXME: BETTER CHECK THIS
    LONGS_EQUAL(-86400000, timeB.getDifferenceInMilliSeconds(&timeA));
}
