/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "testbed/TestbedNetworkInterface.h"
#include "testAPI/mocks/ARAClientMock.h"


TESTBED_NAMESPACE_BEGIN

TEST_GROUP(NetworkInterfaceTest) {
    ARAClientMock* client;
    TestbedNetworkInterface* interface;
    dessert_meshif_t* dessertInterface;
    dessert_meshif_t* otherDessertInterface;

    void setup() {
        client = new ARAClientMock();
        dessertInterface = new dessert_meshif_t();
        otherDessertInterface = new dessert_meshif_t();
     //   memcpy(dessertInterface->hwaddr, DESSERT_LOCAL_ADDRESS, 6);
	std::copy(DESSERT_LOCAL_ADDRESS, DESSERT_LOCAL_ADDRESS+6, dessertInterface->hwaddr);
//        memcpy(otherDessertInterface->hwaddr, DESSERT_BROADCAST_ADDRESS, 6);
	std::copy(DESSERT_BROADCAST_ADDRESS, DESSERT_BROADCAST_ADDRESS + 6, otherDessertInterface->hwaddr);
        interface = new TestbedNetworkInterface(dessertInterface, client, client->getPacketFactory(), 400);
    }

    void teardown() {
        delete client;
        delete interface;
        delete dessertInterface;
        delete otherDessertInterface;
    }
};

TEST(NetworkInterfaceTest, equals) {
    TestbedNetworkInterface* otherInterface = new TestbedNetworkInterface(dessertInterface, client, client->getPacketFactory(), 600);
    CHECK(interface->equals(otherInterface));
    delete otherInterface;
}

TEST(NetworkInterfaceTest, notEquals) {
    TestbedNetworkInterface* ethInterface = new TestbedNetworkInterface(otherDessertInterface, client, client->getPacketFactory(), 400);
    CHECK_FALSE(interface->equals(ethInterface));
    delete ethInterface;
}

TEST(NetworkInterfaceTest, registration) {
    CHECK(interface->isRegistered());
}

TESTBED_NAMESPACE_END
