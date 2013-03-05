/*
 * $FU-Copyright$
 */

#include "NetworkInterfaceMock.h"
#include "AddressMock.h"

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

NetworkInterfaceMock::NetworkInterfaceMock(AbstractARAClient* client) : ReliableNetworkInterface(client, AddressPtr(new AddressMock("DEFAULT")), AddressPtr(new AddressMock("BROADCAST"))) {
    this->name = "NetworkInterfaceMock";
}

NetworkInterfaceMock::NetworkInterfaceMock(const string interfaceName, AbstractARAClient* client) : ReliableNetworkInterface(client, AddressPtr(new AddressMock("DEFAULT")), AddressPtr(new AddressMock("BROADCAST")))  {
    this->name = interfaceName;
}

NetworkInterfaceMock::NetworkInterfaceMock(const string interfaceName, const string localAddressName, AbstractARAClient* client) : ReliableNetworkInterface(client, AddressPtr(new AddressMock(localAddressName)), AddressPtr(new AddressMock("BROADCAST"))) {
    this->name = interfaceName;
}

NetworkInterfaceMock::~NetworkInterfaceMock() {
    while(sentPackets.empty() == false) {
        Pair<Packet*, AddressPtr>* removedPair = sentPackets.back();
        sentPackets.pop_back();
        delete removedPair->getLeft();  // this packet has been cloned in the send method
        delete removedPair;
    }
}

std::string NetworkInterfaceMock::getName() {
    return this->name;
}

std::deque<Pair<Packet*, AddressPtr>*>* NetworkInterfaceMock::getSentPackets() {
    return &sentPackets;
}

void NetworkInterfaceMock::send(const Packet* packet, std::shared_ptr<Address> recipient) {
    Packet* copyOfPacket = packet->clone();
    std::shared_ptr<Address> copyOfAddress = std::shared_ptr<Address>(recipient);
    Pair<Packet*, AddressPtr>* pair = new Pair<Packet*, AddressPtr>(copyOfPacket, copyOfAddress);
    sentPackets.push_back(pair);
}

void NetworkInterfaceMock::broadcast(const Packet* packet) {
    std::shared_ptr<Address> broadCastAddress (new AddressMock("BROADCAST"));
    send(packet, broadCastAddress);
}

bool NetworkInterfaceMock::hasPacketBeenBroadCasted(Packet* packet) {
    for (auto& pair: sentPackets) {
        Packet* currentPacket = pair->getLeft();
        AddressPtr recipient = pair->getRight();

        if(currentPacket->equals(packet)) {
            if(isBroadcastAddress(recipient)) {
                return true;
            }
        }
    }

    return false;
}

bool NetworkInterfaceMock::hasPacketBeenSent(Packet* packet) {
    for (auto& pair: sentPackets) {
        Packet* currentPacket = pair->getLeft();

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
    return 0;
}

deque<Packet*> NetworkInterfaceMock::getUnacknowledgedPackets() const {
    return deque<Packet*>();
}

} /* namespace ARA */
