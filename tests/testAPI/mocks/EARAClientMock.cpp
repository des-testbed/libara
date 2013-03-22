/*
 * $FU-Copyright$
 */

#include "EARAClientMock.h"
#include "EnergyAwareRoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"
#include "testAPI/mocks/LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/time/ClockMock.h"

#include <sstream>

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

EARAClientMock::EARAClientMock() {
    float initialPhi = 10.0;
    float deltaPhi = 5.0;
    Configuration configuration = Configuration(
            new LinearEvaporationPolicyMock(),
            new LinearPathReinforcementPolicy(deltaPhi),
            new BestPheromoneForwardingPolicy(),
            initialPhi
    );
    initialize(configuration, new EnergyAwareRoutingTable());
}

void EARAClientMock::receivePacket(Packet* packet, NetworkInterface* interface) {
    storeReceivedPacket(packet, interface);
    AbstractEARAClient::receivePacket(packet, interface);
}

void EARAClientMock::handleRouteFailure(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    storeRouteFailurePacket(packet, nextHop, interface);
    AbstractEARAClient::handleRouteFailure(packet, nextHop, interface);
}

void EARAClientMock::deliverToSystem(const Packet* packet) {
    storeDeliveredPacket(packet);
}

void EARAClientMock::packetNotDeliverable(const Packet* packet) {
    storeUndeliverablePacket(packet);
}

NetworkInterfaceMock* EARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    NetworkInterfaceMock* mock = AbstractClientMockBase::createNewNetworkInterfaceMock(localAddressName);
    addNetworkInterface(mock);
    return mock;
}

PacketTrap* EARAClientMock::getPacketTrap() {
    return packetTrap;
}

RoutingTable* EARAClientMock::getRoutingTable() {
    return routingTable;
}

} /* namespace ARA */
