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
    if ( (packetType == PacketType::FANT || packetType == PacketType::BANT || packetType == PacketType::PEANT) && isDirectedToThisNode(packet) == false) {
        float routeEnergy = calculateInitialEnergyValue((EARAPacket*)packet);
        routingTable->updateEnergyValue(packet->getSource(), packet->getSender(), interface, routeEnergy);
        handleAntPacketWithDelayTimer(packet, routeEnergy);
    }
    else {
        AbstractARAClient::handleAntPacket(packet, interface);
    }
}

void AbstractEARAClient::handleAntPacketWithDelayTimer(Packet* antPacket, float energyFitnessOfNewAnt) {
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

void AbstractEARAClient::handleDataPacketForThisNode(Packet* packet) {
    AbstractARAClient::handleDataPacketForThisNode(packet);

    unsigned int differenceInEnergyLevel = energyLevelWhenLastPEANTHasBeenSent - getCurrentEnergyLevel();
    if (differenceInEnergyLevel / (float) maximumEnergyValue >= peantEnergyThreshold) {
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
