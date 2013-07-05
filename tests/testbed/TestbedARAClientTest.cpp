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

TEST(TestbedARAClientTest, equals) {
    CHECK(1==1);
}

TESTBED_NAMESPACE_END
