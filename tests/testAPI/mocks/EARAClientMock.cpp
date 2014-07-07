/*
 * $FU-Copyright$
 */

#include "EARAClientMock.h"
#include "EARAPacketFactory.h"
#include "EnergyAwareRoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"
#include "EnergyAwareStochasticForwardingPolicy.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/time/ClockMock.h"

ARA_NAMESPACE_BEGIN

EARAClientMock::EARAClientMock() {
    BasicEARAConfiguration configuration = getStandardConfiguration();
    initializeEARA(configuration);
    currentEnergyLevel = maximumEnergyValue;
}

EARAClientMock::EARAClientMock(EARAConfiguration& configuration) {
    initializeEARA(configuration);
    currentEnergyLevel = maximumEnergyValue;
}

BasicEARAConfiguration EARAClientMock::getStandardConfiguration() const {
    float initialPhi = 10.0;
    float deltaPhi = 5.0;
    int maxNrOfRouteDiscoveryRetries = 2;
    unsigned int routeDiscoveryTimeoutInMilliSeconds = 1000;
    unsigned int packetDeliveryDelayInMilliSeconds = 5;

    unsigned int maxEnergyValue = 100;
    unsigned int maxBatteryCapacityInNetwork = maxEnergyValue;
    float influenceOfMinimumEnergyValue = 3;
    unsigned int routeDiscoveryDelayInMilliSeconds = 2;

    EnergyAwareRoutingTable* routingTable = new EnergyAwareRoutingTable();
    std::shared_ptr<PacketTrap> packetTrap = std::make_shared<PacketTrap>(routingTable);

    return BasicEARAConfiguration(
        routingTable,
        new EARAPacketFactory(15),
        new ExponentialEvaporationPolicyMock(),
        new LinearPathReinforcementPolicy(deltaPhi),
        new EnergyAwareStochasticForwardingPolicy(routingTable),
        packetTrap,
        initialPhi,
        maxNrOfRouteDiscoveryRetries,
        routeDiscoveryTimeoutInMilliSeconds,
        packetDeliveryDelayInMilliSeconds,
        maxEnergyValue,
        maxBatteryCapacityInNetwork,
        influenceOfMinimumEnergyValue,
        routeDiscoveryDelayInMilliSeconds
    );
}

void EARAClientMock::receivePacket(Packet* packet, NetworkInterface* interface) {
    storeReceivedPacket(packet, interface);
    AbstractEARAClient::receivePacket(packet, interface);
}

bool EARAClientMock::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    storeRouteFailurePacket(packetFactory->makeClone(packet), nextHop, interface);
    return AbstractEARAClient::handleBrokenLink(packet, nextHop, interface);
}

void EARAClientMock::deliverToSystem(const Packet* packet) {
    storeDeliveredPacket(packet);
}

void EARAClientMock::packetNotDeliverable(const Packet* packet) {
    storeUndeliverablePacket(packet);
}

unsigned int EARAClientMock::getCurrentEnergyLevel() {
    return currentEnergyLevel;
}

NetworkInterfaceMock* EARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    NetworkInterfaceMock* mock = AbstractClientMockBase::createNewNetworkInterfaceMock(localAddressName);
    addNetworkInterface(mock);
    return mock;
}

void EARAClientMock::setEnergy(unsigned int newEnergyLevel) {
    currentEnergyLevel = newEnergyLevel;
}

std::shared_ptr<PacketTrap> EARAClientMock::getPacketTrap() {
    return packetTrap;
}

EnergyAwareRoutingTable* EARAClientMock::getRoutingTable() {
    return (EnergyAwareRoutingTable*) routingTable;
}

EARAPacketFactory* EARAClientMock::getPacketFactory() {
    return packetFactory;
}

bool EARAClientMock::isRouteDiscoveryDelayTimerExpired(AddressPtr destination) {
    if (runningRouteDiscoveryDelayTimers.find(destination) != runningRouteDiscoveryDelayTimers.end()) {
        if ((runningRouteDiscoveryDelayTimers[destination]).use_count() != 0) {
            return false;
        }
    }
    return true;
}

bool EARAClientMock::isRouteDiscoveryDelayTimerType(AddressPtr destination, TimerType type) {
    if (runningRouteDiscoveryDelayTimers.find(destination) != runningRouteDiscoveryDelayTimers.end()) {
        if ((runningRouteDiscoveryDelayTimers[destination])->getType() == type) {
            return true;
        }
    }
    return false;
}

bool EARAClientMock::isRouteDiscoveryDelayTimerRunning(AddressPtr destination) {
    if (runningRouteDiscoveryDelayTimers.find(destination) != runningRouteDiscoveryDelayTimers.end()) {
        std::shared_ptr<TimerMock> timer = std::dynamic_pointer_cast<TimerMock>(runningRouteDiscoveryDelayTimers[destination]);
        return timer->isRunning();
    }
    return false;
}

void EARAClientMock::expireRouteDiscoveryDelayTimer(AddressPtr destination) {
    if (runningRouteDiscoveryDelayTimers.find(destination) != runningRouteDiscoveryDelayTimers.end()) {
        std::shared_ptr<TimerMock> timer = std::dynamic_pointer_cast<TimerMock>(runningRouteDiscoveryDelayTimers[destination]);
        timer->expire();
    }
}

ARA_NAMESPACE_END
