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
    maximumBatteryCapacityInNetwork = configuration.getMaximumBatteryCapacityInNetwork(); //TODO read this from configuration
    influenceOfMinimumEnergyValue = configuration.getInfluenceOfMinimumEnergyValue();
    routeDiscoveryDelayInMilliSeconds = configuration.getRouteDiscoveryDelayInMilliSeconds();
    peantEnergyThreshold = configuration.getPEANTEnergyThreshold();
    energyLevelWhenLastPEANTHasBeenSent = maximumEnergyValue;
}

AbstractEARAClient::~AbstractEARAClient() {
    // delete running delivery timers
    for (RouteDiscoveryDelayTimerMap::iterator iterator=runningRouteDiscoveryDelayTimers.begin(); iterator!=runningRouteDiscoveryDelayTimers.end(); iterator++) {
        Timer* timer = iterator->second;
        AntPacketRouteFitness* bestAnt = (AntPacketRouteFitness*) timer->getContextObject();
        delete bestAnt->packet;
        delete bestAnt;
        delete timer;
    }
    runningRouteDiscoveryDelayTimers.clear();
}

void AbstractEARAClient::createNewRouteFrom(Packet* packet, NetworkInterface* interface) {
    float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
    float initialEnergyValue = calculateInitialEnergyValue(static_cast<EARAPacket*>(packet));
    routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue, initialEnergyValue);
    //TODO log energy value (in percent)
    logTrace("Created new route to %s via %s (phi=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue);
}

void AbstractEARAClient::updateRoutingTable(Packet* packet, NetworkInterface* interface) {
    AddressPtr source = packet->getSource();
    AddressPtr sender = packet->getSender();
    if (packet->isAntPacket() && isLocalAddress(packet->getPreviousHop()) == false
        && routingTable->isNewRoute(source, sender, interface) == false) {

        // trigger the evaporation first so this does not effect the new route or update
        routingTable->triggerEvaporation();

        // the route for this ant packet needs to be reinforced so we can completely replace the energy values
        float currentPheromoneValue = routingTable->getPheromoneValue(source, sender, interface);
        float newPheromoneValue = pathReinforcementPolicy->calculateReinforcedValue(currentPheromoneValue);

        float newEnergyValue = calculateInitialEnergyValue(static_cast<EARAPacket*>(packet));
        routingTable->update(source, sender, interface, newPheromoneValue, newEnergyValue);
    }
    else {
        AbstractARAClient::updateRoutingTable(packet, interface);
    }
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

        if (isDirectedToThisNode(packet) == false && routeDiscoveryDelayInMilliSeconds > 0) {
            handleAntPacketWithDelayTimer(packet, interface);
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
    assert(nrOfHops >= 0);

    if (nrOfHops == 0) {
        // packet has been directly received from the source so we return the maximum normalized energy
        return 10;
    }
    else {
        unsigned int totalEnergy = packet->getTotalEnergyValue();
        unsigned int minimumEnergy = packet->getMinimumEnergyValue();
        float averageEnergy = totalEnergy / (float) nrOfHops;

        // some asserts for easy debugging
        assert(totalEnergy > 0);
        assert(minimumEnergy > 0);
        assert(minimumEnergy <= averageEnergy);

        float normalizedAverage = normalizeEnergyValue(averageEnergy);
        float normalizedMinimum = normalizeEnergyValue(minimumEnergy);

        float initialEnergyValue = normalizedAverage - ( (normalizedAverage - normalizedMinimum) / influenceOfMinimumEnergyValue );
        assert(initialEnergyValue >= 1 && initialEnergyValue <= 10);
        return initialEnergyValue;
    }
}

float AbstractEARAClient::normalizeEnergyValue(float energyValue) const {
    if (energyValue > maximumBatteryCapacityInNetwork) {
        logError("Configuration error: Evaluating an energy value which is greater than the maximum configured energy capacity of a nodes battery in the network");
        energyValue = maximumBatteryCapacityInNetwork;
    }
    // the returned value lies in the interval (1, 10)
    return (energyValue / (float) maximumBatteryCapacityInNetwork) * 9 + 1;
}

float AbstractEARAClient::getEnergyPercentage(float energyValue) const {
    return (energyValue - 1) * 100 / 9.0;
}

void AbstractEARAClient::handleAntPacketWithDelayTimer(Packet* antPacket, NetworkInterface* interface) {
    RouteDiscoveryDelayTimerMap::iterator found = runningRouteDiscoveryDelayTimers.find(antPacket->getSource());
    float routeEnergyOfNewAnt = routingTable->getEnergyValue(antPacket->getSource(), antPacket->getSender(), interface);

    if (found == runningRouteDiscoveryDelayTimers.end()) {
        startNewRouteDiscoveryDelayTimer(antPacket, routeEnergyOfNewAnt);
    }
    else {
        Timer* delayTimer = found->second;
        AntPacketRouteFitness* bestAnt = (AntPacketRouteFitness*) delayTimer->getContextObject();

        float routeFitnessOfNewAnt = calculateRouteFitness(antPacket->getTTL(), routeEnergyOfNewAnt);
        if (routeFitnessOfNewAnt > bestAnt->routeEnergyFitness) {
            delete bestAnt->packet;
            bestAnt->packet = antPacket;
            bestAnt->routeEnergyFitness = routeFitnessOfNewAnt;
        }
        else {
            delete antPacket;
        }
    }
}

void AbstractEARAClient::startNewRouteDiscoveryDelayTimer(Packet* antPacket, float routeEnergyOfNewAnt) {
    AntPacketRouteFitness* contextObject = new AntPacketRouteFitness();
    contextObject->packet = antPacket;
    contextObject->routeEnergyFitness = calculateRouteFitness(antPacket->getTTL(), routeEnergyOfNewAnt);

    Timer* newDelayTimer = getNewTimer(TimerType::ROUTE_DISCOVERY_DELAY_TIMER, contextObject);
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
    AntPacketRouteFitness* bestAnt = (AntPacketRouteFitness*) timer->getContextObject();
    runningRouteDiscoveryDelayTimers.erase(bestAnt->packet->getSource());
    broadCast(bestAnt->packet);
    delete bestAnt;
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
