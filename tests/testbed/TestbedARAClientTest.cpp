/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "TestbedARAClient.h"


TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedARAClientTest) {
    TestbedARAClient* client;

    void setup() {
        client = new TestbedARAClient();
    }

    void teardown() {
        delete client;
    }
};

/**
 * Test checks instantiation, if the test compiles you may instantiate a TestbedARAClient.
 * Checks for memory leaks in general use of client.
 */
TEST(TestbedARAClientTest, instantiation) {
    TestbedARAClient client = TestbedARAClient();
}

TESTBED_NAMESPACE_END
