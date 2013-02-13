#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/TimeMock.h"

#include <iostream>

using namespace ARA;

TEST_GROUP(TimeMockTest) {};

TEST(TimeMockTest, testUsleep) {
    /// create a time mock
    TimeMock time = TimeMock();
    /// check if the timestamp is initialized with '0'
    int seconds = time.getTimestamp()->tv_sec;
    BYTES_EQUAL(0, seconds);
    long int milliseconds = time.getTimestamp()->tv_usec;
    BYTES_EQUAL(0, milliseconds);
    /// test the usleep method
    time.usleep(1000);
    seconds = time.toSeconds();
    BYTES_EQUAL(1, seconds);
    milliseconds = time.toMilliseconds();
    LONGS_EQUAL(1000, milliseconds);
}
