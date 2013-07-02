/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "testbed/NetworkInterface.h"
#include "testAPI/mocks/ARAClientMock.h"


TESTBED_NAMESPACE_BEGIN

TEST_GROUP(NetworkInterfaceTest) {
    ARAClientMock* client;
    NetworkInterface* interface;

    void setup() {
        client = new ARAClientMock();
        interface = new NetworkInterface("eth0", client, client->getPacketFactory(), 400);
    }

    void teardown() {
        delete client;
        delete interface;
    }
};

TEST(NetworkInterfaceTest, notEquals) {
    NetworkInterface* tapInterface = new NetworkInterface("tap1", client, client->getPacketFactory(), 400);
    CHECK_FALSE(interface->equals(tapInterface));
    delete tapInterface;
}

TEST(NetworkInterfaceTest, equals) {
    NetworkInterface* ethInterface = new NetworkInterface("eth0", client, client->getPacketFactory(), 400);
    CHECK(interface->equals(ethInterface));
    delete ethInterface;
}

TESTBED_NAMESPACE_END
