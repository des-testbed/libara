/*
 * $FU-Copyright$
 */

#include "AbstractClientMockBase.h"
#include "Environment.h"

#include <sstream>

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

AbstractClientMockBase::~AbstractClientMockBase() {
    // delete the NetworkInterfaceMocks that have been created via createNewNetworkInterfaceMock
    while(interfaceMocks.empty() == false) {
        NetworkInterfaceMock* mock = interfaceMocks.back();
        interfaceMocks.pop_back();
        delete mock;
    }

    for(auto& pair: receivedPackets) {
        delete pair;
    }

    for(auto& packetInfo: routeFailurePackets) {
        delete packetInfo.packet;
    }

    for(auto& packet: undeliverablePackets) {
        delete packet;
    }
}

void AbstractClientMockBase::storeReceivedPacket(Packet* packet, NetworkInterface* interface) {
    receivedPackets.push_back(new Pair<const Packet*, const NetworkInterface*>(packet, interface));
}

void AbstractClientMockBase::storeRouteFailurePacket(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    PacketInfo packetInfo;
    packetInfo.packet = packet;
    packetInfo.nextHop = nextHop;
    packetInfo.interface = interface;
    routeFailurePackets.push_back(packetInfo);
}

void AbstractClientMockBase::storeDeliveredPacket(const Packet* packet) {
    deliveredPackets.push_back(packet);
}

void AbstractClientMockBase::storeUndeliverablePacket(const Packet* packet) {
    undeliverablePackets.push_back(packet);
}

NetworkInterfaceMock* AbstractClientMockBase::createNewNetworkInterfaceMock(const std::string localAddressName) {
    std::stringstream mockName;
    mockName << "InterfaceMock" << (interfaceMocks.size()+1);
    NetworkInterfaceMock* mock = new NetworkInterfaceMock(mockName.str().c_str(), localAddressName, this);
    interfaceMocks.push_back(mock);
    return mock;
}

std::deque<const Packet*>* AbstractClientMockBase::getDeliveredPackets() {
    return &deliveredPackets;
}

int AbstractClientMockBase::getNumberOfReceivedPackets() {
    return receivedPackets.size();
}

std::deque<Pair<const Packet*, const NetworkInterface*>*> AbstractClientMockBase::getReceivedPackets() {
    return receivedPackets;
}

int AbstractClientMockBase::getNumberOfRouteFailures() {
    return routeFailurePackets.size();
}

std::deque<AbstractClientMockBase::PacketInfo> AbstractClientMockBase::getRouteFailurePackets() {
    return routeFailurePackets;
}

int AbstractClientMockBase::getNumberOfUndeliverablePackets() {
    return undeliverablePackets.size();
}

std::deque<const Packet*> AbstractClientMockBase::getUndeliverablePackets() {
    return undeliverablePackets;
}

} /* namespace ARA */
