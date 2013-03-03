#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/TimeMock.h"

using namespace ARA;

TEST_GROUP(TimeMockTest) {
    void setup() {
        TimeMock::setCurrentTime(0, 0);
    }
};

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

TEST(TimeMockTest, getDifferenceInMilliSeconds) {
    TimeMock time1 = TimeMock();
    TimeMock time2 = TimeMock();

    LONGS_EQUAL(0, time1.getDifferenceInMilliSeconds(&time2));

    time1 = TimeMock(30);
    time2 = TimeMock(20);
    LONGS_EQUAL(10000, time1.getDifferenceInMilliSeconds(&time2));

    time1 = TimeMock(40);
    time2 = TimeMock(50);
    LONGS_EQUAL(-10000, time1.getDifferenceInMilliSeconds(&time2));

    time1 = TimeMock(2);
    time2 = TimeMock(1, 600);
    LONGS_EQUAL(400, time1.getDifferenceInMilliSeconds(&time2));

    time1 = TimeMock(2, 300);
    time2 = TimeMock(2, 300);
    LONGS_EQUAL(0, time1.getDifferenceInMilliSeconds(&time2));
}

TEST(TimeMockTest, letTimePass) {
    TimeMock time = TimeMock();

    TimeMock::letTimePass(1000);
    time.setToCurrentTime();
    LONGS_EQUAL(1, time.getSeconds());
    LONGS_EQUAL(0, time.getMilliSeconds());

    TimeMock::letTimePass(500);
    time.setToCurrentTime();
    LONGS_EQUAL(1, time.getSeconds());
    LONGS_EQUAL(500, time.getMilliSeconds());

    TimeMock::letTimePass(700);
    time.setToCurrentTime();
    LONGS_EQUAL(2, time.getSeconds());
    LONGS_EQUAL(200, time.getMilliSeconds());

    TimeMock::letTimePass(800);
    time.setToCurrentTime();
    LONGS_EQUAL(3, time.getSeconds());
    LONGS_EQUAL(0, time.getMilliSeconds());
}
