/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "UnixTime.h"

using namespace ARA;

TEST_GROUP(UnixTimeTest) {};

TEST(UnixTimeTest, create) {
    UnixTime time = UnixTime(123, 456);
    timeval timeStamp = time.getTimestamp();
    LONGS_EQUAL(123, timeStamp.tv_sec);
    LONGS_EQUAL(456, timeStamp.tv_usec);
}

TEST(UnixTimeTest, createWithTimeval) {
    timeval newTimeval;
    newTimeval.tv_sec = 789;
    newTimeval.tv_usec = 1001;
    UnixTime time = UnixTime(newTimeval);

    timeval returnedTimeStamp = time.getTimestamp();
    LONGS_EQUAL(newTimeval.tv_sec, returnedTimeStamp.tv_sec);
    LONGS_EQUAL(newTimeval.tv_usec, returnedTimeStamp.tv_usec);
}

TEST(UnixTimeTest, equality) {
    UnixTime originalTime = UnixTime(123, 456);
    UnixTime anotherTime = UnixTime(456, 123);
    UnixTime sameTime = UnixTime(123, 456);

    CHECK_FALSE(originalTime == anotherTime);
    CHECK_TRUE(originalTime == originalTime);
    CHECK_TRUE(originalTime == sameTime);
    CHECK_TRUE(sameTime == originalTime);
}

TEST(UnixTimeTest, copyConstructor) {
    UnixTime originalTime = UnixTime(123, 456);
    UnixTime timeCopy = UnixTime(originalTime);

    CHECK(originalTime == timeCopy);
}

TEST(UnixTimeTest, getSeconds) {
    UnixTime time = UnixTime(123, 456);
    LONGS_EQUAL(123, time.getSeconds());
}

TEST(UnixTimeTest, getMilliSeconds) {
    UnixTime time = UnixTime(123, 456);
    LONGS_EQUAL(456, time.getMilliSeconds());
}

TEST(UnixTimeTest, getDifferenceInMilliSeconds) {
    UnixTime timeA = UnixTime(20, 200);
    UnixTime timeB = UnixTime(30, 300);

    LONGS_EQUAL(10100, timeB.getDifferenceInMilliSeconds(&timeA));
}
