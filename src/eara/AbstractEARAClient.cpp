/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "TimerType.h"

#include <cassert>
#include <cmath>

ARA_NAMESPACE_BEGIN

AbstractEARAClient::AbstractEARAClient(EARAConfiguration& configuration) {
    initializeEARA(configuration);
}

void AbstractEARAClient::initializeEARA(EARAConfiguration& configuration) {
    AbstractARAClient::initialize(configuration);
    routingTable = configuration.getEnergyAwareRoutingTable();
    packetFactory = configuration.getEARAPacketFactory();
    forwardingPolicy = configuration.getForwardingPolicy();
    maximumEnergyValue = configuration.getMaximumEnergyValue();
    influenceOfMinimumEnergyValue = configuration.getInfluenceOfMinimumEnergyValue();
    routeDiscoveryDelayInMilliSeconds = configuration.getRouteDiscoveryDelayInMilliSeconds();
    peantEnergyThreshold = configuration.getPEANTEnergyThreshold();
    energyLevelWhenLastPEANTHasBeenSent = maximumEnergyValue;
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

void AbstractEARAClient::broadCast(Packet* packet) {
    EARAPacket* earaPacket = static_cast<EARAPacket*>(packet);
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
    if ( hasBeenSentByThisNode(packet) == false
         && (packetType == PacketType::FANT || packetType == PacketType::BANT || packetType == PacketType::PEANT)) {
        float routeEnergy = calculateInitialEnergyValue(static_cast<EARAPacket*>(packet));
        routingTable->updateEnergyValue(packet->getSource(), packet->getSender(), interface, routeEnergy);

        if (isDirectedToThisNode(packet) == false && routeDiscoveryDelayInMilliSeconds > 0) {
            handleAntPacketWithDelayTimer(packet, routeEnergy);
        }
        else {
            AbstractARAClient::handleAntPacket(packet, interface);
        }
    }
    else {
        AbstractARAClient::handleAntPacket(packet, interface);
    }
}

float AbstractEARAClient::calculateInitialEnergyValue(EARAPacket* packet) {
    int nrOfHops = packetFactory->getMaximumNrOfHops() - packet->getTTL();
    assert(nrOfHops > 0);

    if (nrOfHops == 1) {
        // packet has been directly received from the source so we return the maximum normalized energy
        return 10;
    }
    else {
        nrOfHops -= 1; // don't count in the last hop, because we also don't count in the energy of the current node
        unsigned int totalEnergy = packet->getTotalEnergyValue();
        unsigned int minimumEnergy = packet->getMinimumEnergyValue();

        // some asserts for easy debugging
        assert(totalEnergy > 0);
        assert(minimumEnergy > 0);

        float averageEnergy = totalEnergy / (float) nrOfHops;
        float normalizedAverage = normalizeEnergyValue(averageEnergy);
        float normalizedMinimum = normalizeEnergyValue(minimumEnergy);

        float initialEnergyValue = normalizedAverage - ( (normalizedAverage - normalizedMinimum) / influenceOfMinimumEnergyValue );
        assert(initialEnergyValue >= 1 && initialEnergyValue <= 10);
        return initialEnergyValue;
    }
}

float AbstractEARAClient::normalizeEnergyValue(float energyValue) const {
    // the returned value lies in the interval (1, 10)
    return (energyValue / (float) maximumEnergyValue) * 9 + 1;
}

float AbstractEARAClient::getEnergyPercentage(float energyValue) const {
    return (energyValue - 1) * 100 / 9.0;
}

void AbstractEARAClient::handleAntPacketWithDelayTimer(Packet* antPacket, float energyFitnessOfNewAnt) {
    RouteDiscoveryDelayTimerMap::iterator found = runningRouteDiscoveryDelayTimers.find(antPacket->getSource());

    if (found == runningRouteDiscoveryDelayTimers.end()) {
        startNewRouteDiscoveryDelayTimer(antPacket);
    }
    else {
        Timer* delayTimer = found->second;
        EARAPacket* bestAntPacket = static_cast<EARAPacket*>(delayTimer->getContextObject());

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
    float potentiatedPheromoneValue = pow(ttl, forwardingPolicy->getPheromoneWeight());
    float potentiatedEnergyValue = pow(energyFitness, forwardingPolicy->getEnergyWeight());

    return potentiatedPheromoneValue * potentiatedEnergyValue;
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

void AbstractEARAClient::handleDataPacketForThisNode(Packet* packet) {
    AbstractARAClient::handleDataPacketForThisNode(packet);

    unsigned int currentEnergy = getCurrentEnergyLevel();
    unsigned int differenceInEnergyLevel = energyLevelWhenLastPEANTHasBeenSent - currentEnergy;
    if (differenceInEnergyLevel / (float) maximumEnergyValue >= peantEnergyThreshold) {
        energyLevelWhenLastPEANTHasBeenSent = currentEnergy;
        broadcastPEANT();
    }
}

void AbstractEARAClient::broadcastPEANT() {
    logDebug("Sending new PEANT over all interfaces");
    for(auto& interface: interfaces) {
        AddressPtr source = interface->getLocalAddress();
        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* peant = packetFactory->makePEANT(source, sequenceNr);
        interface->broadcast(peant);
    }
}

ARA_NAMESPACE_END
