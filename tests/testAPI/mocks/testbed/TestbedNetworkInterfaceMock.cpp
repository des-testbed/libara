/*
 * $FU-Copyright$
 */

#include "TestbedNetworkInterfaceMock.h"

using namespace std;

TESTBED_NAMESPACE_BEGIN

TestbedNetworkInterfaceMock::TestbedNetworkInterfaceMock(std::string interfaceName, AbstractNetworkClient* client, std::shared_ptr<TestbedAddress> localAddress, std::shared_ptr<TestbedAddress> broadcastAddress, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds) : TestbedNetworkInterface(interfaceName, client, localAddress, broadcastAddress, packetFactory, ackTimeoutInMicroSeconds) {

}


void TestbedNetworkInterfaceMock::doSend(const Packet* packet, AddressPtr recipient) {
    Packet* clone = packetFactory->makeClone(packet);
    std::pair<const Packet*, std::shared_ptr<Address>> pair(clone, recipient);
    sentPackets.push_back(pair);
//    TestbedNetworkInterface::doSend(packet, recipient);
}

unsigned int TestbedNetworkInterfaceMock::getNumberOfSentPackets() {
    return sentPackets.size();
}

TESTBED_NAMESPACE_END
