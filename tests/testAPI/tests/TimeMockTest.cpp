#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/TimeMock.h"

using namespace ARA;

TEST_GROUP(TimeMockTest) {};

TEST(TimeMockTest, createWithParameter) {
    TimeMock time = TimeMock(123, 456);

    LONGS_EQUAL(123, time.getSeconds());
    LONGS_EQUAL(456, time.getMilliSeconds());
}

TEST(TimeMockTest, createWithoutParameter) {
    TimeMock time = TimeMock();

    LONGS_EQUAL(0, time.getSeconds());
    LONGS_EQUAL(0, time.getMilliSeconds());
}

TEST(TimeMockTest, letTimePass) {
    TimeMock time = TimeMock();

    time.letTimePass(1000);
    LONGS_EQUAL(1, time.getSeconds());
    LONGS_EQUAL(0, time.getMilliSeconds());

    time.letTimePass(500);
    LONGS_EQUAL(1, time.getSeconds());
    LONGS_EQUAL(500, time.getMilliSeconds());

    time.letTimePass(700);
    LONGS_EQUAL(2, time.getSeconds());
    LONGS_EQUAL(200, time.getMilliSeconds());

    time.letTimePass(800);
    LONGS_EQUAL(3, time.getSeconds());
    LONGS_EQUAL(0, time.getMilliSeconds());
}
