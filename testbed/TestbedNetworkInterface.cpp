/*
 * $FU-Copyright$
 */

#include "TestbedNetworkInterface.h"
#include "TestbedAddress.h"
#include "TestbedPacketDispatcher.h"

TESTBED_NAMESPACE_BEGIN

TestbedNetworkInterface::TestbedNetworkInterface(dessert_meshif_t* dessertPointer, AbstractARAClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds) 
  : ReliableNetworkInterface(client, ackTimeoutInMicroSeconds, std::make_shared<TestbedAddress>(dessertPointer->hwaddr), std::make_shared<TestbedAddress>(DESSERT_BROADCAST_ADDRESS)) { 
  // DEBUG:  std::cout << "[TestbedNetworkInterface] address: " << *localAddress << " broadcast address: " << *broadcastAddress << std::endl;  
}

bool TestbedNetworkInterface::equals(NetworkInterface* otherInterface) {
    if (TestbedNetworkInterface* other = dynamic_cast<TestbedNetworkInterface*>(otherInterface)){
        return (this->getLocalAddress() == other->getLocalAddress());
    }

    return false;
}

void TestbedNetworkInterface::doSend(const Packet* packet, std::shared_ptr<Address> recipient){
    _send(packet, std::dynamic_pointer_cast<TestbedAddress>(localAddress), recipient);
}


TESTBED_NAMESPACE_END
