/*
 * $FU-Copyright$
 */

#ifndef TESTBED_NETWORK_INTERFACE_MOCK_H_
#define TESTBED_NETWORK_INTERFACE_MOCK_H_

#include "CLibs.h"
#include "Testbed.h"
#include "TestbedPacket.h"
#include "TestbedAddress.h"
#include "TestbedPacketFactory.h"
#include "TestbedNetworkInterface.h"

#include <utility>

TESTBED_NAMESPACE_BEGIN

class TestbedNetworkInterfaceMock: public TestbedNetworkInterface {
    public:
        TestbedNetworkInterfaceMock(std::string interfaceName, AbstractNetworkClient* client, std::shared_ptr<TestbedAddress> local, std::shared_ptr<TestbedAddress> broadcast, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds);
        /*
        TestbedNetworkInterfaceMock(AbstractNetworkClient* client);
        TestbedNetworkInterfaceMock(const std::string interfaceName, AbstractNetworkClient* client);
        TestbedNetworkInterfaceMock(const std::string interfaceName, const std::string localAddressName, AbstractNetworkClient* client);
        ~TestbedNetworkInterfaceMock();
        TestbedNetworkInterfaceMock(const char* sourceName = "Source", const char* destinationName = "Destination", unsigned int sequenceNumber = 1, int ttl = 20, char packetType = NetworkInterfaceType::DATA);
        TestbedNetworkInterfaceMock(const char* sourceName, const char* destinationName, const char* senderName, unsigned int sequenceNumber = 1, int ttl = 20, char packetType = NetworkInterfaceType::DATA, const char* payload= "Hello World");
        bool operator==(TestbedNetworkInterfaceMock& other) {
            return this->equals(&other);
        }
    private:
        string DEFAULT_INTERFACE_NAME = "NetworkInterfaceMock";
        */
        void doSend(const Packet* packet, std::shared_ptr<Address> recipient);
        unsigned int getNumberOfSentPackets();

    private:
        std::deque<std::pair<const Packet*, std::shared_ptr<Address>> > sentPackets;
        std::deque<const Packet*> broadcastedPackets;
};

TESTBED_NAMESPACE_END

#endif 
