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
    // this number of hops will not include the last hop to this node, because this is only counted after the routing table update
    int nrOfHops = packetFactory->getMaximumNrOfHops() - packet->getTTL();
    assert(nrOfHops >= 0);

    if (nrOfHops == 0) {
        // packet has been directly received from the source
        return packet->getTotalEnergyValue() / (float) maximumEnergyValue;
    }
    else {
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

bool AbstractEARAClient::hasBeenReceivedEarlier(const Packet* packet) {
    if (runningRouteDiscoveryDelayTimers.find(packet->getSource()) == runningRouteDiscoveryDelayTimers.end()) {
        return AbstractARAClient::hasBeenReceivedEarlier(packet);
    }
    else {
        // there is a route discovery delay timer running so we need to give this packet a chance
        return false;
    }
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

void AbstractEARAClient::handleFANTorBANT(Packet* antPacket) {
    AddressPtr source = antPacket->getSource();
    RouteDiscoveryDelayTimerMap::iterator found = runningRouteDiscoveryDelayTimers.find(source);

    if (found == runningRouteDiscoveryDelayTimers.end()) {
        // no delay timer is yet running, so we start a new one
        Timer* newDelayTimer = getNewTimer(TimerType::ROUTE_DISCOVERY_DELAY_TIMER, antPacket);
        newDelayTimer->addTimeoutListener(this);
        newDelayTimer->run(routeDiscoveryDelayInMilliSeconds * 1000);
        runningRouteDiscoveryDelayTimers[source] = newDelayTimer;
    }
    else {
        // there is already a timer for this ant generation running
        Timer* delayTimer = found->second;
        //TODO we could cache this value so it doesn't need to be recalculated over and over
        Packet* bestAntPacket = (Packet*) delayTimer->getContextObject();
        float bestRouteFitnessSoFar = calculateRouteFitness(bestAntPacket);
        float routeFitnessOfNewAnt = calculateRouteFitness(antPacket);

        if (routeFitnessOfNewAnt > bestRouteFitnessSoFar) {
            delete bestAntPacket;
            delayTimer->setContextObject(antPacket);
        }
        else {
            delete antPacket;
        }
    }
}

float AbstractEARAClient::calculateRouteFitness(Packet* packet) {
    float energyFitnessOfNewAnt = calculateInitialEnergyValue((EARAPacket*)packet);
    //TODO add weighting parameters (use from forwarding policy)
    return packet->getTTL() * energyFitnessOfNewAnt;
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

float AbstractEARAClient::reinforcePheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    float currentPheromoneValue = routingTable->getPheromoneValue(destination, nextHop, interface);
    float newPheromoneValue = pathReinforcementPolicy->calculateReinforcedValue(currentPheromoneValue);
    routingTable->update(destination, nextHop, interface, newPheromoneValue);
    return newPheromoneValue;
}

ARA_NAMESPACE_END
