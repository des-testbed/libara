/*
 * $FU-Copyright$
 */

#include "TestbedNetworkInterface.h"
#include "TestbedAddress.h"
#include "TestbedPacketDispatcher.h"

#include <thread>

TESTBED_NAMESPACE_BEGIN

TestbedNetworkInterface::TestbedNetworkInterface(dessert_meshif_t* dessertPointer, AbstractNetworkClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds) 
  : ReliableNetworkInterface(client, ackTimeoutInMicroSeconds, std::make_shared<TestbedAddress>(dessertPointer->hwaddr), std::make_shared<TestbedAddress>(DESSERT_BROADCAST_ADDRESS)) { 
  // DEBUG:  std::cerr << "[TestbedNetworkInterface] address: " << *localAddress << " broadcast address: " << *broadcastAddress << std::endl;  
}

bool TestbedNetworkInterface::equals(NetworkInterface* otherInterface) {
    if (TestbedNetworkInterface* other = dynamic_cast<TestbedNetworkInterface*>(otherInterface)){
        return (this->getLocalAddress()->toString().compare(other->getLocalAddress()->toString()) == 0);
    }

    return false;
}

void TestbedNetworkInterface::receive(Packet* packet) {
    std::unique_lock<std::mutex> lock(receiveMutex);
    ReliableNetworkInterface::receive(packet);
}

void TestbedNetworkInterface::send(const Packet* packet, AddressPtr recipient) {
    assert(packet != nullptr);

    std::unique_lock<std::mutex> packetsLock(unacknowledgedPacketsMutex);

    const TestbedPacket *testbedPacket = dynamic_cast<const TestbedPacket*>(packet);
    //unacknowledgedPackets.push_back(packet);
    unacknowledgedPackets.push_back(testbedPacket);
    packetsLock.unlock();

    //doSend(packet, recipient);
    doSend(testbedPacket, recipient);

    std::lock_guard<std::mutex> timerLock(acknowledgmentTimerMutex);
    //ReliableNetworkInterface::startAcknowledgmentTimer(packet, recipient);
    ReliableNetworkInterface::startAcknowledgmentTimer(testbedPacket, recipient);
}

void TestbedNetworkInterface::doSend(const Packet* packet, std::shared_ptr<Address> recipient){
    std::thread::id this_id = std::this_thread::get_id();
    std::cerr << "[TestbedNetworkInterface::doSend]  thread id " << this_id << std::endl;
    // DEBUG:
    std::cerr << "[TestbedNetworkInterface::doSend]  send packet " <<  packet->getSequenceNumber() << " to " << recipient->toString() << std::endl;
    assert(packet != nullptr);
    std::unique_lock<std::mutex> lock(sendMutex);
    dispatch(packet, std::dynamic_pointer_cast<TestbedAddress>(localAddress), recipient);
}


TESTBED_NAMESPACE_END
