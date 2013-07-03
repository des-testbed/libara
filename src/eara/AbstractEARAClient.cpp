/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "TimerType.h"

#include <cassert>

ARA_NAMESPACE_BEGIN

AbstractEARAClient::AbstractEARAClient(EARAConfiguration& configuration) {
    initializeEARA(configuration);
}

void AbstractEARAClient::initializeEARA(EARAConfiguration& configuration) {
    AbstractARAClient::initialize(configuration);
    routingTable = configuration.getEnergyAwareRoutingTable();
    packetFactory = configuration.getEARAPacketFactory();
    maximumEnergyValue = configuration.getMaximumEnergyValue();
    influenceOfMinimumEnergyValue = configuration.getInfluenceOfMinimumEnergyValue();
    routeDiscoveryDelayInMilliSeconds = configuration.getRouteDiscoveryDelayInMilliSeconds();
}

AbstractEARAClient::~AbstractEARAClient() {
    // delete running delivery timers
    for (RouteDiscoveryDelayTimerMap::iterator iterator=runningRouteDiscoveryDelayTimers.begin(); iterator!=runningRouteDiscoveryDelayTimers.end(); iterator++) {
        Timer* timer = iterator->second;
        delete (Packet*) timer->getContextObject();
        delete timer;
    }
    runningRouteDiscoveryDelayTimers.clear();
}

void AbstractEARAClient::createNewRouteFrom(Packet* packet, NetworkInterface* interface) {
    float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
    float initialEnergyValue = calculateInitialEnergyValue((EARAPacket*)packet);
    routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue, initialEnergyValue);
    logTrace("Created new route to %s via %s (phi=%.2f, energy=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue, initialEnergyValue);
}

float AbstractEARAClient::calculateInitialEnergyValue(EARAPacket* packet) {
    int nrOfHops = packetFactory->getMaximumNrOfHops() - packet->getTTL();
    assert(nrOfHops > 0);

    if (nrOfHops == 1) {
        // packet has been directly received from the source
        return packet->getTotalEnergyValue();
    }
    else {
        // decrease number of hops, because the energy of the last hop (current node has not been added to the packet)
        nrOfHops -= 1;
        float averageValue = packet->getTotalEnergyValue() / (float) nrOfHops;
        float averagePercent = averageValue / (float) maximumEnergyValue;
        float minimumPercent = packet->getMinimumEnergyValue() / (float) maximumEnergyValue;

        return averagePercent - ( (averagePercent - minimumPercent) / influenceOfMinimumEnergyValue );
    }
}

void AbstractEARAClient::broadCast(Packet* packet) {
    EARAPacket* earaPacket = (EARAPacket*) packet;
    unsigned int energyOfCurrentNode = getCurrentEnergyLevel();
    earaPacket->addEnergyValue(energyOfCurrentNode);
    AbstractARAClient::broadCast(earaPacket);
}

void AbstractEARAClient::handleAntPacket(Packet* packet) {
    char packetType = packet->getType();
    if ( (packetType == PacketType::FANT || packetType == PacketType::BANT) && isDirectedToThisNode(packet) == false) {
        handleFANTorBANT(packet);
    }
    else {
        AbstractARAClient::handleAntPacket(packet);
    }
}

void AbstractEARAClient::handleFANTorBANT(Packet* packet) {
    AddressPtr source = packet->getSource();
    RouteDiscoveryDelayTimerMap::iterator delayTimer;
    delayTimer = runningRouteDiscoveryDelayTimers.find(source);
    if (delayTimer == runningRouteDiscoveryDelayTimers.end()) {
        // no delay timer is yet running, so we start a new one
        Timer* newDelayTimer = getNewTimer(TimerType::ROUTE_DISCOVERY_DELAY_TIMER, packet);
        newDelayTimer->addTimeoutListener(this);
        newDelayTimer->run(routeDiscoveryDelayInMilliSeconds * 1000);
        runningRouteDiscoveryDelayTimers[source] = newDelayTimer;
    }
    else {
        // there is already a timer for this ant generation running
        float energyFitnessOfNewAnt = calculateInitialEnergyValue((EARAPacket*)packet);
        // TODO calculate fitness of new ant packet
        // TODO compare to existing ant packet
        // TODO if it is better then replace the existing one (don#t forget to delete)
        // TODo if not then delete the new packet
    }
}

void AbstractEARAClient::timerHasExpired(Timer* responsibleTimer) {
    char timerType = responsibleTimer->getType();
    switch (timerType) {
        case TimerType::ROUTE_DISCOVERY_DELAY_TIMER:
            handleExpiredRouteDiscoveryDelayTimer(responsibleTimer);
            return;
        default:
            AbstractARAClient::timerHasExpired(responsibleTimer);
    }
}

void AbstractEARAClient::handleExpiredRouteDiscoveryDelayTimer(Timer* timer) {
    Packet* bestAntPacket = (Packet*) timer->getContextObject();
    runningRouteDiscoveryDelayTimers.erase(bestAntPacket->getSource());
    broadCast(bestAntPacket);
    delete timer;
}

ARA_NAMESPACE_END
