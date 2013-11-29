/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "StandardTime.h"

ARA_NAMESPACE_BEGIN

TEST_GROUP(StandardTimeTest) {
    std::chrono::time_point<std::chrono::system_clock> timestampA;
    std::chrono::time_point<std::chrono::system_clock> timestampB;

    void setup() {
        // we just get the current time
        time_t rawtime;
        time(&rawtime);

        // pass it to our own timeval data structure
        tm* timeinfo = localtime(&rawtime);

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

TEST(StandardTimeTest, create) {
    StandardTime time(timestampA);

    LONGS_EQUAL(1372773722, time.getSeconds());
    LONGS_EQUAL(1372773722000, time.getMilliSeconds());
}

TEST(StandardTimeTest, equality) {
    StandardTime originalTime = StandardTime(timestampA);
    StandardTime anotherTime = StandardTime(timestampB);
    StandardTime sameTime = StandardTime(timestampA);

    CHECK_FALSE(originalTime == anotherTime);
    CHECK_TRUE(originalTime == originalTime);
    CHECK_TRUE(originalTime == sameTime);
    CHECK_TRUE(sameTime == originalTime);
}

TEST(StandardTimeTest, copyConstructor) {
    StandardTime originalTime = StandardTime(timestampA);
    StandardTime timeCopy = StandardTime(originalTime);

    CHECK(originalTime == timeCopy);
}
TEST(StandardTimeTest, getSeconds) {
    StandardTime time = StandardTime(timestampA);
    LONGS_EQUAL(1372773722, time.getSeconds());
}

TEST(StandardTimeTest, getMilliSeconds) {
    StandardTime time = StandardTime(timestampA);
    LONGS_EQUAL(1372773722000, time.getMilliSeconds());
}

TEST(StandardTimeTest, getDifferenceInMilliSeconds) {
    StandardTime timeA = StandardTime(timestampA);
    StandardTime timeB = StandardTime(timestampB);
    /// FIXME: BETTER CHECK THIS
    LONGS_EQUAL(-86400000, timeB.getDifferenceInMilliSeconds(&timeA));
}

ARA_NAMESPACE_END
