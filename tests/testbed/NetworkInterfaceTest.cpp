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
    dessert_meshif_t* dessertInterface;

    void setup() {
        client = new ARAClientMock();
        dessertInterface = new dessert_meshif_t();
        memcpy(dessertInterface->hwaddr, DESSERT_LOCAL_ADDRESS, 6);
        interface = new NetworkInterface(dessertInterface, client, client->getPacketFactory(), 400);
    }

    void teardown() {
        delete client;
        delete interface;
        delete dessertInterface;
    }
};

TEST(NetworkInterfaceTest, equals) {
    NetworkInterface* otherInterface = new NetworkInterface(dessertInterface, client, client->getPacketFactory(), 600);
    CHECK(interface->equals(otherInterface));
    delete otherInterface;
}

TEST(NetworkInterfaceTest, notEquals) {
    NetworkInterface* ethInterface = new NetworkInterface(nullptr, client, client->getPacketFactory(), 400);
    CHECK_FALSE(interface->equals(ethInterface));
    delete ethInterface;
}

TESTBED_NAMESPACE_END
