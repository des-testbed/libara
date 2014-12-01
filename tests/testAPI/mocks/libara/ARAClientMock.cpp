/*
 * $FU-Copyright$
 */

#include "ARAClientMock.h"
#include "BasicConfiguration.h"
#include "RoutingTableEntry.h"
#include "PacketFactory.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"
#include "testAPI/mocks/libara/RoutingTableMock.h"
#include "testAPI/mocks/libara/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/libara/time/ClockMock.h"

#include <sstream>

ARA_NAMESPACE_BEGIN

ARAClientMock::ARAClientMock() {
    BasicConfiguration configuration = getStandardConfiguration();
    initialize(configuration);
}

ARAClientMock::ARAClientMock(Configuration& configuration) {
    initialize(configuration);
}

BasicConfiguration ARAClientMock::getStandardConfiguration() const {
    float initialPhi = 5.0;
    float deltaPhi = 5.0;
    RoutingTable* routingTable = new RoutingTableMock();
    std::shared_ptr<PacketTrap> packetTrap = std::make_shared<PacketTrap>(routingTable);

    return BasicConfiguration(
        routingTable,
        new PacketFactory(15),
        new ExponentialEvaporationPolicyMock(),
        new LinearPathReinforcementPolicy(deltaPhi),
        new BestPheromoneForwardingPolicy(routingTable),
        packetTrap,
        initialPhi
    );
}

void ARAClientMock::receivePacket(Packet* packet, NetworkInterface* interface) {
    storeReceivedPacket(packet, interface);
    AbstractARAClient::receivePacket(packet, interface);
}

bool ARAClientMock::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    storeRouteFailurePacket(packetFactory->makeClone(packet), nextHop, interface);
    return AbstractARAClient::handleBrokenLink(packet, nextHop, interface);
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

std::shared_ptr<PacketTrap> ARAClientMock::getPacketTrap() {
    return packetTrap;
}

RoutingTable* ARAClientMock::getRoutingTable() {
    return routingTable;
}

void ARAClientMock::setMaxHopCount(int n) {
    packetFactory->setMaxHopCount(n);
}

double ARAClientMock::getInitialPhi() const {
    return initialPheromoneValue;
}

unsigned int ARAClientMock::getPacketDeliveryDelay() const {
    return packetDeliveryDelayInMilliSeconds;
}

std::weak_ptr<Timer> ARAClientMock::getNeighborActivityTimer() const {
    return neighborActivityTimer;
}

void ARAClientMock::forget(AddressPtr neighbor) {
    // delete all known routes via this next hop
    std::deque<RoutingTableEntryTupel> allRoutesOverNeighbor = routingTable->getAllRoutesThatLeadOver(neighbor);
    for (auto& route: allRoutesOverNeighbor) {
        routingTable->removeEntry(route.destination, neighbor, route.entry->getNetworkInterface());
    }

    NeighborActivityMap::const_iterator foundNeighbor = neighborActivityTimes.find(neighbor);
    if(foundNeighbor != neighborActivityTimes.end()) {
        // delete the associated Time object first
        delete foundNeighbor->second.first;
        neighborActivityTimes.erase(neighbor);
    }
}

bool ARAClientMock::isPANTsTimerExpired(AddressPtr destination) {
    if (scheduledPANTs.find(destination) != scheduledPANTs.end()) {
        if ((scheduledPANTs[destination]).use_count() != 0) {
            return false;
        }
    }
    return true;
}

bool ARAClientMock::isPANTsTimerType(AddressPtr destination, char type) {
    if (scheduledPANTs.find(destination) != scheduledPANTs.end()) {
        if ((scheduledPANTs[destination])->getType() == type) {
            return true;
        }
    }
    return false;
}

bool ARAClientMock::isPANTsTimerRunning(AddressPtr destination) {
    if (scheduledPANTs.find(destination) != scheduledPANTs.end()) {
        std::shared_ptr<TimerMock> timer = std::dynamic_pointer_cast<TimerMock>(scheduledPANTs[destination]);
        return timer->isRunning();
    }
    return false;
}

void ARAClientMock::expirePANTsTimer(AddressPtr destination) {
    if (scheduledPANTs.find(destination) != scheduledPANTs.end()) {
        std::shared_ptr<TimerMock> timer = std::dynamic_pointer_cast<TimerMock>(scheduledPANTs[destination]);
        timer->expire();
    }
}

ARA_NAMESPACE_END
