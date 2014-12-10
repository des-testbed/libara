/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "CLibs.h"

#include "TestbedPacket.h"

#include "testAPI/mocks/libara/ARAClientMock.h"

#include "testAPI/mocks/testbed/TestbedNetworkInterfaceMock.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedNetworkInterfaceTest) {
    ARAClientMock* client;
    //NetworkInterfaceMock* interface;
    TestbedNetworkInterfaceMock* interface;
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
        interface = new TestbedNetworkInterfaceMock(wifi, client, client->getPacketFactory(), 400);
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


//
TEST(TestbedNetworkInterfaceTest, doNotAcknowledgeAckPackets) {
    std::shared_ptr<AddressMock> source = std::make_shared<AddressMock>("source");
    std::shared_ptr<AddressMock> destination = std::make_shared<AddressMock>("destination");
    std::shared_ptr<AddressMock> sender = std::make_shared<AddressMock>("sender");

    unsigned int sequenceNr = 123;

    TestbedPacket* packet = new TestbedPacket(source, destination, sender, PacketType::ACK, sequenceNr, 1);

    interface->receive(packet);
    // we should never acknowledge acknowledgment packets
    BYTES_EQUAL(0, interface->getNumberOfSentPackets());
}

/*
TEST(TestbedNetworkInterfaceTest, registration) {
    CHECK(interface->isRegistered());
}
*/

TESTBED_NAMESPACE_END
