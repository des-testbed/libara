/*
 * $FU-Copyright$
 */

#include "ARAClientMock.h"
#include "RoutingTableEntry.h"
#include "BestPheromoneForwardingPolicy.h"
#include "testAPI/mocks/LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/TimeFactoryMock.h"

#include <sstream>

namespace ARA {

ARAClientMock::ARAClientMock() : AbstractARAClient(new TimeFactoryMock()) {
    forwardingPolicy = new BestPheromoneForwardingPolicy();
    forwardingPolicy->setRoutingTable(routingTable);
    setEvaporationPolicy(new LinearEvaporationPolicyMock());
}

ARAClientMock::~ARAClientMock() {
    delete forwardingPolicy;

    // delete the NetworkInterfaceMocks that have been created via createNewNetworkInterfaceMock
    while(interfaceMocks.empty() == false) {
        NetworkInterfaceMock* mock = interfaceMocks.back();
        interfaceMocks.pop_back();
        delete mock;
    }

    for(auto& pair: receivedPackets) {
        delete pair;
    }

    routingTable->setEvaporationPolicy(NULL);
    delete evaporationPolicy;
}

void ARAClientMock::receivePacket(Packet* packet, NetworkInterface* interface) {
    receivedPackets.push_back(new Pair<const Packet*, const NetworkInterface*>(packet, interface));
    AbstractARAClient::receivePacket(packet, interface);
}

void ARAClientMock::setEvaporationPolicy(EvaporationPolicy *policy){
    evaporationPolicy = policy;
    routingTable->setEvaporationPolicy(evaporationPolicy);
};

ForwardingPolicy* ARAClientMock::getForwardingPolicy() {
    return forwardingPolicy;
}

void ARAClientMock::updateRoutingTable(const Packet* packet, NetworkInterface* interface) {
    routingTable->update(packet->getSource(), packet->getSender(), interface, 10);
}

void ARAClientMock::deliverToSystem(const Packet* packet) {
    deliveredPackets.push_back(packet);
}

NetworkInterfaceMock* ARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    std::stringstream mockName;
    mockName << "InterfaceMock" << (interfaceMocks.size()+1);
    NetworkInterfaceMock* mock = new NetworkInterfaceMock(mockName.str().c_str(), localAddressName);
    interfaceMocks.push_back(mock);
    addNetworkInterface(mock);
    return mock;
}

PacketTrap* ARAClientMock::getPacketTrap() {
    return packetTrap;
}

RoutingTable* ARAClientMock::getRoutingTable() {
    return routingTable;
}

std::deque<const Packet*>* ARAClientMock::getDeliveredPackets() {
    return &deliveredPackets;
}

int ARAClientMock::getNumberOfReceivedPackets() {
    return receivedPackets.size();
}

std::deque<Pair<const Packet*, const NetworkInterface*>*> ARAClientMock::getReceivedPackets() {
    return receivedPackets;
}

} /* namespace ARA */
