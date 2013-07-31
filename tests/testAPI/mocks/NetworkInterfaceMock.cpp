/*
 * $FU-Copyright$
 */

#include "NetworkInterfaceMock.h"
#include "AddressMock.h"
#include "Environment.h"

using namespace std;

ARA_NAMESPACE_BEGIN

string NetworkInterfaceMock::DEFAULT_INTERFACE_NAME = "NetworkInterfaceMock";
AddressPtr NetworkInterfaceMock::DEFAULT_LOCAL_ADDRESS = AddressPtr(new AddressMock("DEFAULT"));
AddressPtr NetworkInterfaceMock::DEFAULT_BROADCAST_ADDRESS = AddressPtr(new AddressMock("BROADCAST"));
int NetworkInterfaceMock::DEFAULT_ACK_TIMEOUT = 5000;

NetworkInterfaceMock::NetworkInterfaceMock(AbstractNetworkClient* client) : ReliableNetworkInterface(client, DEFAULT_ACK_TIMEOUT, DEFAULT_LOCAL_ADDRESS, DEFAULT_BROADCAST_ADDRESS) {
    this->name = DEFAULT_INTERFACE_NAME;
}

NetworkInterfaceMock::NetworkInterfaceMock(const string interfaceName, AbstractNetworkClient* client) : ReliableNetworkInterface(client, DEFAULT_ACK_TIMEOUT, DEFAULT_LOCAL_ADDRESS, DEFAULT_BROADCAST_ADDRESS)  {
    this->name = interfaceName;
}

NetworkInterfaceMock::NetworkInterfaceMock(const string interfaceName, const string localAddressName, AbstractNetworkClient* client) : ReliableNetworkInterface(client, DEFAULT_ACK_TIMEOUT, AddressPtr(new AddressMock(localAddressName)), AddressPtr(new AddressMock("BROADCAST"))) {
    this->name = interfaceName;
}

NetworkInterfaceMock::~NetworkInterfaceMock() {
    while(sentPackets.empty() == false) {
        Pair<const Packet*, AddressPtr>* removedPair = sentPackets.back();
        sentPackets.pop_back();
        delete removedPair->getLeft(); // delete the clone of the sent packet
        delete removedPair;
    }

    for(auto& packet: broadcastedPackets) {
        delete packet;
    }
}

std::string NetworkInterfaceMock::getName() {
    return this->name;
}

std::deque<Pair<const Packet*, AddressPtr>*>* NetworkInterfaceMock::getSentPackets() {
    return &sentPackets;
}

void NetworkInterfaceMock::doSend(const Packet* packet, AddressPtr recipient) {
    Packet* clone = packetFactory->makeClone(packet);
    sentPackets.push_back(new Pair<const Packet*, AddressPtr>(clone, recipient));
}

/**
 * This is implemented here again to prevent the ReliableNetworkInterface from deleting the packet yet.
 * We want to store the packet until the destructor is called just for our convenience in the unit tests.
 */
void NetworkInterfaceMock::broadcast(const Packet* packet) {
    doSend(packet, broadcastAddress);
    broadcastedPackets.push_back(packet);
}

bool NetworkInterfaceMock::hasPacketBeenBroadCasted(Packet* packet) {
    for (auto& currentPacket: broadcastedPackets) {
        if(currentPacket->equals(packet)) {
            return true;
        }
    }

    return false;
}

bool NetworkInterfaceMock::hasPacketBeenSent(Packet* packet) {
    for (auto& pair: sentPackets) {
        const Packet* currentPacket = pair->getLeft();

        if(currentPacket->equals(packet)) {
            return true;
        }
    }

    return false;
}

bool NetworkInterfaceMock::equals(NetworkInterface* otherInterface) {
    NetworkInterfaceMock* otherInterfaceMock = dynamic_cast<NetworkInterfaceMock*>(otherInterface);
    if(otherInterfaceMock == NULL) {
        return false;
    }
    else {
        return this->name.compare(otherInterfaceMock->name) == 0;
    }
}

unsigned int NetworkInterfaceMock::getNumberOfSentPackets() {
    return sentPackets.size();
}

int NetworkInterfaceMock::getNrOfUnacknowledgedPackets() const {
    return unacknowledgedPackets.size();
}

int NetworkInterfaceMock::getNrOfRunningTimers() const {
    return runningTimers.size();
}

ARA_NAMESPACE_END
