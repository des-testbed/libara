/*
 * $FU-Copyright$
 */

#include "ARAClientMock.h"
#include "BasicConfiguration.h"
#include "RoutingTableEntry.h"
#include "PacketFactory.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/time/ClockMock.h"

#include <sstream>

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

ARAClientMock::ARAClientMock() {
    float initialPhi = 5.0;
    float deltaPhi = 5.0;
    BasicConfiguration configuration = BasicConfiguration(
            new ExponentialEvaporationPolicyMock(),
            new LinearPathReinforcementPolicy(deltaPhi),
            new BestPheromoneForwardingPolicy(),
            initialPhi
    );
    initialize(configuration, new RoutingTable(), new PacketFactory());
}

void ARAClientMock::receivePacket(Packet* packet, NetworkInterface* interface) {
    storeReceivedPacket(packet, interface);
    AbstractARAClient::receivePacket(packet, interface);
}

void ARAClientMock::handleRouteFailure(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    storeRouteFailurePacket(packetFactory->makeClone(packet), nextHop, interface);
    AbstractARAClient::handleRouteFailure(packet, nextHop, interface);
}

void ARAClientMock::deliverToSystem(const Packet* packet) {
    storeDeliveredPacket(packet);
}

void ARAClientMock::packetNotDeliverable(const Packet* packet) {
    storeUndeliverablePacket(packet);
}

NetworkInterfaceMock* ARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    NetworkInterfaceMock* mock = AbstractClientMockBase::createNewNetworkInterfaceMock(localAddressName);
    addNetworkInterface(mock);
    return mock;
}

PacketTrap* ARAClientMock::getPacketTrap() {
    return packetTrap;
}

RoutingTable* ARAClientMock::getRoutingTable() {
    return routingTable;
}

void ARAClientMock::setMaxHopCount(int n) {
    maxHopCount = n;
}

double ARAClientMock::getInitialPhi() const {
    return initialPheromoneValue;
}

} /* namespace ARA */
