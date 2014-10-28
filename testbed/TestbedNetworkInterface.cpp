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
  numberOfReceivedPackets = numberOfSentPackets = 0;
  interfaceName = std::string(dessertPointer->if_name);
  // DEBUG:  std::cerr << "[TestbedNetworkInterface] address: " << *localAddress << " broadcast address: " << *broadcastAddress << std::endl;  
}

bool TestbedNetworkInterface::equals(NetworkInterface* otherInterface) {
    if (TestbedNetworkInterface* other = dynamic_cast<TestbedNetworkInterface*>(otherInterface)){
        return (this->getLocalAddress()->toString().compare(other->getLocalAddress()->toString()) == 0);
    }

    return false;
}

std::string TestbedNetworkInterface::getInterfaceName(){
    return interfaceName;
}

std::string TestbedNetworkInterface::getStatistics(){
    std::ostringstream result;
    float percentage = .0;
 
    // print the received packet statistics
    for (auto metric = receiveStatistics.begin(); metric != receiveStatistics.end(); ++metric) {
         percentage = ((float)metric->second/(float)numberOfReceivedPackets) * 100.0;
         result << metric->first << ": " << metric->second << " ("  << percentage <<  " %)" << std::endl; 
    }
    result << "total number of received packets: " << numberOfReceivedPackets << std::endl;
    result << std::endl;

    // print the sent packet statistics
    for (auto metric = sentStatistics.begin(); metric != sentStatistics.end(); ++metric) {
         percentage = ((float)metric->second/(float)numberOfSentPackets) * 100.0;
         result << metric->first << ": " << metric->second << " ("  << percentage <<  " %)" << std::endl; 
    }
    result << "total number of sent packets: " << numberOfSentPackets << std::endl;
    result << std::endl;

    return result.str();
}

void TestbedNetworkInterface::receive(Packet* packet) {
    assert(packet != nullptr);
    std::unique_lock<std::mutex> lock(receiveMutex);
    packetCounter(packet->getType(), true);
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
    packetCounter(packet->getType(), false);
    doSend(testbedPacket, recipient);

    std::lock_guard<std::mutex> timerLock(acknowledgmentTimerMutex);
    //ReliableNetworkInterface::startAcknowledgmentTimer(packet, recipient);
    ReliableNetworkInterface::startAcknowledgmentTimer(testbedPacket, recipient);
}

void TestbedNetworkInterface::doSend(const Packet* packet, std::shared_ptr<Address> recipient){
    assert(packet != nullptr);
    // DEBUG:  std::thread::id this_id = std::this_thread::get_id();
    // DEBUG: std::cerr << "[TestbedNetworkInterface::doSend]  thread id " << this_id << std::endl;
    // DEBUG: std::cerr << "[TestbedNetworkInterface::doSend]  send packet " <<  packet->getSequenceNumber() << " to " << recipient->toString() << std::endl;
    std::unique_lock<std::mutex> lock(sendMutex);
    dispatch(packet, std::dynamic_pointer_cast<TestbedAddress>(localAddress), recipient);
}


void TestbedNetworkInterface::packetCounter(char type, bool isReceivedPacket){
    std::string key = PacketType::getAsString(type);

    if (isReceivedPacket) {
        receiveStatistics[key] = ++receiveStatistics[key];
        numberOfReceivedPackets++;
    } else {
        sentStatistics[key] = ++sentStatistics[key];
        numberOfSentPackets++;
    }
}

void TestbedNetworkInterface::timerHasExpired(std::weak_ptr<Timer> ackTimer) {
    std::unique_lock<std::mutex> timerLock(acknowledgmentTimerMutex);
    ReliableNetworkInterface::timerHasExpired(ackTimer);
}

TESTBED_NAMESPACE_END
