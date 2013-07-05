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
    //FIXME why do we need this method if it does the same as the original one?!
    // the energy value will be set separately when processing FANTs/BANTs below
    float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
    routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue);
    logTrace("Created new route to %s via %s (phi=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue);
}

float AbstractEARAClient::calculateInitialEnergyValue(EARAPacket* packet) {
    int nrOfHops = packetFactory->getMaximumNrOfHops() - packet->getTTL();
    assert(nrOfHops > 0);

    if (nrOfHops == 1) {
        // packet has been directly received from the source
        return packet->getTotalEnergyValue() / (float) maximumEnergyValue;
    }
    else {
        nrOfHops -= 1; // don't count in the last hop, because we also don't coun't in the energy of the current node
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

void AbstractEARAClient::handleAntPacket(Packet* packet, NetworkInterface* interface) {
    char packetType = packet->getType();
    if ( (packetType == PacketType::FANT || packetType == PacketType::BANT) && isDirectedToThisNode(packet) == false) {
        float routeEnergy = calculateInitialEnergyValue((EARAPacket*)packet);
        routingTable->updateEnergyValue(packet->getSource(), packet->getSender(), interface, routeEnergy);
        handleFANTorBANT(packet, routeEnergy);
    }
    else {
        AbstractARAClient::handleAntPacket(packet, interface);
    }
}

void AbstractEARAClient::handleFANTorBANT(Packet* antPacket, float energyFitnessOfNewAnt) {
    RouteDiscoveryDelayTimerMap::iterator found = runningRouteDiscoveryDelayTimers.find(antPacket->getSource());

    if (found == runningRouteDiscoveryDelayTimers.end()) {
        startNewRouteDiscoveryDelayTimer(antPacket);
    }
    else {
        Timer* delayTimer = found->second;
        EARAPacket* bestAntPacket = (EARAPacket*) delayTimer->getContextObject();

        //TODO we could cache this value so it doesn't need to be recalculated over and over
        float routeEnergyOfBestAnt = calculateInitialEnergyValue(bestAntPacket);
        float bestRouteFitnessSoFar = calculateRouteFitness(bestAntPacket->getTTL(), routeEnergyOfBestAnt);
        float routeFitnessOfNewAnt = calculateRouteFitness(antPacket->getTTL(), energyFitnessOfNewAnt);

        if (routeFitnessOfNewAnt > bestRouteFitnessSoFar) {
            delete bestAntPacket;
            delayTimer->setContextObject(antPacket);
        }
        else {
            delete antPacket;
        }
    }
}

void AbstractEARAClient::startNewRouteDiscoveryDelayTimer(Packet* antPacket) {
    Timer* newDelayTimer = getNewTimer(TimerType::ROUTE_DISCOVERY_DELAY_TIMER, antPacket);
    newDelayTimer->addTimeoutListener(this);
    newDelayTimer->run(routeDiscoveryDelayInMilliSeconds * 1000);
    runningRouteDiscoveryDelayTimers[antPacket->getSource()] = newDelayTimer;
}

float AbstractEARAClient::calculateRouteFitness(int ttl, float energyFitness) {
    //TODO add weighting parameters (use from forwarding policy)
    return ttl * energyFitness;
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

void AbstractEARAClient::handleDataPacketForThisNode(Packet* packet) {
    AbstractARAClient::handleDataPacketForThisNode(packet);
    //TODO get the current energy level
    // check if the difference between the current value and this value has exceeded the threshold
    // if yes then send a new PEANT
    // if not do nothing else
}

ARA_NAMESPACE_END
