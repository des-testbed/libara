/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "cEnvirMock.h"

using namespace ARA::test::omnetpp;

TEST_GROUP(cEnvirMockTest) {};

TEST(cEnvirMockTest, create) {
    cEnvirMock environmentMock = cEnvirMock();
}
