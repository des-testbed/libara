/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "CLibs.h"

#include "TestbedNetworkInterface.h"

#include "testAPI/mocks/ARAClientMock.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedNetworkInterfaceTest) {
    ARAClientMock* client;
    TestbedNetworkInterface* interface;
    /// dessert interface
    dessert_meshif_t* wifi;

    void setup() {
        /// setup the dessert interface
        u_char mac[ETHER_ADDR_LEN] = {0x00,0x1f,0x1f,0x09,0x09,0xe2};
        char name[6] = "wlan0";
        wifi = new dessert_meshif_t();
        
        std::copy(mac, mac + ETHER_ADDR_LEN, wifi->hwaddr);
        std::copy(name, name + 6, wifi->if_name);


        client = new ARAClientMock();
        interface = new TestbedNetworkInterface(wifi, client, client->getPacketFactory(), 400);
    }

    void teardown() {
        delete client;
        delete interface;
        delete wifi;
    }
};

TEST(TestbedNetworkInterfaceTest, equals) {
    TestbedNetworkInterface* otherInterface = new TestbedNetworkInterface(wifi, client, client->getPacketFactory(), 600);
    CHECK(interface->equals(otherInterface));
    delete otherInterface;
}

TEST(TestbedNetworkInterfaceTest, notEquals) {
    /// setup another dessert interface
    u_char mac[ETHER_ADDR_LEN] = {0x00,0x1f,0x1f,0x09,0x06,0xe9};
    dessert_meshif_t* anotherDessertInterface = new dessert_meshif_t();
    std::copy(mac, mac + ETHER_ADDR_LEN, anotherDessertInterface->hwaddr);

    TestbedNetworkInterface* anotherInterface = new TestbedNetworkInterface(anotherDessertInterface, client, client->getPacketFactory(), 400);
    CHECK_FALSE(interface->equals(anotherInterface));

    delete anotherInterface;
    delete anotherDessertInterface;
}

TEST(TestbedNetworkInterfaceTest, getInterfaceName) {
    std::string name("wlan0");
    CHECK(interface->getInterfaceName().compare(name) == 0);
}

/*
TEST(TestbedNetworkInterfaceTest, registration) {
    CHECK(interface->isRegistered());
}
*/

TESTBED_NAMESPACE_END
