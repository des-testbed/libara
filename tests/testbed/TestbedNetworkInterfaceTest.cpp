/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"
#include "testbed/TestbedMain.h"

#include "testAPI/mocks/TestbedNetworkInterfaceMock.h"
#include "testAPI/mocks/ARAClientMock.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedNetworkInterfaceTest) {
    ARAClientMock* client;
    dessert_meshif_t* meshInterface;
    dessert_meshif_t* anotherMeshInterface;
    TestbedNetworkInterfaceMock* interface;

    void setup() {
        client = new ARAClientMock();

        u_char address[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
        meshInterface = new dessert_meshif_t();
        std::copy(address, address+6, meshInterface->hwaddr);

        u_char otherAddress[ETHER_ADDR_LEN] = {6,5,4,3,2,1};
        anotherMeshInterface = new dessert_meshif_t();
        std::copy(otherAddress, otherAddress + 6, anotherMeshInterface->hwaddr);

        interface = new TestbedNetworkInterfaceMock(meshInterface, client);
    }

    void teardown() {
        delete client;
        delete meshInterface;
        delete anotherMeshInterface;
        delete interface;
    }
};

TEST(TestbedNetworkInterfaceTest, equals) {
    TestbedNetworkInterfaceMock* otherInterface = new TestbedNetworkInterfaceMock(meshInterface, client);
    CHECK(interface->equals(otherInterface));
    delete otherInterface;
}

TEST(TestbedNetworkInterfaceTest, notEquals) {
    TestbedNetworkInterfaceMock* otherInterface = new TestbedNetworkInterfaceMock(anotherMeshInterface, client);
    CHECK_FALSE(interface->equals(otherInterface));
    delete otherInterface;
}

TEST(TestbedNetworkInterfaceTest, send) {
  //(const Packet* packet, AddressPtr recipient);
}

TEST(TestbedNetworkInterfaceTest, receive) {
  //void receive(Packet* packet);
}

TESTBED_NAMESPACE_END
