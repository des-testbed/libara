/*
 * $FU-Copyright$
 */

#include "AbstractARAClient.h"
#include "PacketType.h"
#include "LinearPathReinforcementPolicy.h"
#include "Timer.h"
#include "Environment.h"
#include "Exception.h"

#include "sstream"
using namespace std;

namespace ARA {

typedef std::unordered_map<Timer*, RouteDiscoveryInfo> DiscoveryTimerInfo;
typedef std::unordered_map<Timer*, AddressPtr> DeliveryTimerInfo;

AbstractARAClient::AbstractARAClient(Configuration& configuration, RoutingTable *routingTable, PacketFactory* packetFactory) {
    initialize(configuration, routingTable, packetFactory);
}

void AbstractARAClient::initialize(Configuration& configuration, RoutingTable* routingTable, PacketFactory* packetFactory) {
    forwardingPolicy = configuration.getForwardingPolicy();
    pathReinforcementPolicy = configuration.getReinforcementPolicy();
    evaporationPolicy = configuration.getEvaporationPolicy();
    initialPheromoneValue = configuration.getInitialPheromoneValue();
    maxNrOfRouteDiscoveryRetries = configuration.getMaxNrOfRouteDiscoveryRetries();
    routeDiscoveryTimeoutInMilliSeconds = configuration.getRouteDiscoveryTimeoutInMilliSeconds();
    packetDeliveryDelayInMilliSeconds = configuration.getPacketDeliveryDelayInMilliSeconds();

    this->packetFactory = packetFactory;
    this->routingTable = routingTable;
    routingTable->setEvaporationPolicy(evaporationPolicy);

    packetTrap = new PacketTrap(routingTable);
    runningRouteDiscoveries = unordered_map<AddressPtr, Timer*>();
    runningRouteDiscoveryTimers = unordered_map<Timer*, RouteDiscoveryInfo>();
    runningDeliveryTimers = unordered_map<Timer*, AddressPtr>();
}

AbstractARAClient::~AbstractARAClient() {
    // delete logger if it has been set
    if(logger != nullptr) {
        delete logger;
    }

    // delete the sequence number lists of the last received packets
    for (LastReceivedPacketsMap::iterator iterator=lastReceivedPackets.begin(); iterator!=lastReceivedPackets.end(); iterator++) {
        // the addresses are disposed of automatically by shared_ptr
        delete iterator->second;
    }
    lastReceivedPackets.clear();

    // delete the known intermediate hop addresses for all sources
    for (KnownIntermediateHopsMap::iterator iterator=knownIntermediateHops.begin(); iterator!=knownIntermediateHops.end(); iterator++) {
        delete iterator->second;
    }
    knownIntermediateHops.clear();

    // delete running route discovery timers
    for (DiscoveryTimerInfo::iterator iterator=runningRouteDiscoveryTimers.begin(); iterator!=runningRouteDiscoveryTimers.end(); iterator++) {
        delete iterator->first;
    }
    runningRouteDiscoveryTimers.clear();

    // delete running delivery timers
    for (DeliveryTimerInfo::iterator iterator=runningDeliveryTimers.begin(); iterator!=runningDeliveryTimers.end(); iterator++) {
        delete iterator->first;
    }
    runningDeliveryTimers.clear();

    /* The following members may have be deleted earlier, depending on the destructor of the implementing class */
    DELETE_IF_NOT_NULL(packetFactory);
    DELETE_IF_NOT_NULL(packetTrap);
    DELETE_IF_NOT_NULL(routingTable);
    DELETE_IF_NOT_NULL(pathReinforcementPolicy);
    DELETE_IF_NOT_NULL(evaporationPolicy);
    DELETE_IF_NOT_NULL(forwardingPolicy);
}

void AbstractARAClient::setLogger(Logger* logger) {
    this->logger = logger;
}

void AbstractARAClient::logTrace(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_TRACE, args);
    }
}

void AbstractARAClient::logDebug(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_DEBUG, args);
    }
}

void AbstractARAClient::logInfo(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_INFO, args);
    }
}

void AbstractARAClient::logWarn(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_WARN, args);
    }
}

void AbstractARAClient::logError(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_ERROR, args);
    }
}

void AbstractARAClient::logFatal(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_FATAL, args);
    }
}

void AbstractARAClient::addNetworkInterface(NetworkInterface* newInterface) {
    interfaces.push_back(newInterface);
}

NetworkInterface* AbstractARAClient::getNetworkInterface(unsigned int index) {
    return interfaces.at(index);
}

unsigned int AbstractARAClient::getNumberOfNetworkInterfaces() {
    return interfaces.size();
}

PacketFactory* AbstractARAClient::getPacketFactory() const{
    return packetFactory;
}

void AbstractARAClient::sendPacket(Packet* packet) {
    if (packet->getTTL() > 0) {
        AddressPtr destination = packet->getDestination();
        if (routingTable->isDeliverable(packet)) {
            NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
            NetworkInterface* interface = nextHop->getInterface();
            AddressPtr nextHopAddress = nextHop->getAddress();
            packet->setPreviousHop(packet->getSender());
            packet->setSender(interface->getLocalAddress());

            logTrace("Forwarding DATA packet %u from %s to %s via %s", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getDestinationString().c_str(), nextHopAddress->toString().c_str());
            reinforcePheromoneValue(destination, nextHopAddress, interface);

            interface->send(packet, nextHopAddress);
        } else {
            packetTrap->trapPacket(packet);
            if (isRouteDiscoveryRunning(destination) == false) {
                startNewRouteDiscovery(packet);
            }
            else {
                logTrace("Route discovery for %s is already running. Trapping packet %u", destination->toString().c_str(), packet->getSequenceNumber());
            }
        }
    }
    else {
        handlePacketWithZeroTTL(packet);
    }
}

void AbstractARAClient::reinforcePheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    float currentPheromoneValue = routingTable->getPheromoneValue(destination, nextHop, interface);
    float newPheromoneValue = pathReinforcementPolicy->calculateReinforcedValue(currentPheromoneValue);
    routingTable->update(destination, nextHop, interface, newPheromoneValue);
}

void AbstractARAClient::startNewRouteDiscovery(const Packet* packet) {
    logDebug("Packet %u from %s to %s is not deliverable. Starting route discovery phase", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getDestinationString().c_str());

    AddressPtr destination = packet->getDestination();
    if(knownIntermediateHops.find(destination) != knownIntermediateHops.end()) {
        std::unordered_set<AddressPtr>* seenNodesForThisDestination = knownIntermediateHops[packet->getDestination()];
        seenNodesForThisDestination->clear();
    }

    startRouteDiscoveryTimer(packet);
    sendFANT(destination);
}

void AbstractARAClient::sendFANT(AddressPtr destination) {
    unsigned int sequenceNr = getNextSequenceNumber();

    for(auto& interface: interfaces) {
        Packet* fant = packetFactory->makeFANT(interface->getLocalAddress(), destination, sequenceNr);
        interface->broadcast(fant);
    }
}

void AbstractARAClient::startRouteDiscoveryTimer(const Packet* packet) {
    Timer* timer = Environment::getClock()->getNewTimer();
    timer->addTimeoutListener(this);
    timer->run(routeDiscoveryTimeoutInMilliSeconds * 1000);

    RouteDiscoveryInfo discoveryInfo;
    discoveryInfo.nrOfRetries = 0;
    discoveryInfo.timer = timer;
    discoveryInfo.originalPacket = packet;

    runningRouteDiscoveries[packet->getDestination()] = timer;
    runningRouteDiscoveryTimers[timer] = discoveryInfo;
}

bool AbstractARAClient::isRouteDiscoveryRunning(AddressPtr destination) {
    return runningRouteDiscoveries.find(destination) != runningRouteDiscoveries.end();
}

void AbstractARAClient::handlePacketWithZeroTTL(Packet* packet) {
    logWarn("Dropping packet %u from %s because TTL reached zero", packet->getSequenceNumber(), packet->getSourceString().c_str());
    delete packet;
}

void AbstractARAClient::receivePacket(Packet* packet, NetworkInterface* interface) {
    packet->decreaseTTL();
    updateRoutingTable(packet, interface);

    if(hasBeenReceivedEarlier(packet)) {
        handleDuplicatePacket(packet, interface);
    }
    else {
        registerReceivedPacket(packet);
        handlePacket(packet, interface);
    }
}

void AbstractARAClient::handleDuplicatePacket(Packet* packet, NetworkInterface* interface) {
    if(packet->isDataPacket()) {
        sendDuplicateWarning(packet, interface);
    }
    else if(packet->getType() == PacketType::BANT && isDirectedToThisNode(packet)) {
        logInfo("Another BANT %u came back from %s via %s.", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getSenderString().c_str());
    }

    delete packet;
}

void AbstractARAClient::sendDuplicateWarning(Packet* packet, NetworkInterface* interface) {
    AddressPtr sender = interface->getLocalAddress();
    logWarn("Routing loop for packet %u from %s detected. Sending duplicate warning back to %s", packet->getSequenceNumber(), packet->getSourceString().c_str(), packet->getSenderString().c_str());
    Packet* duplicateWarningPacket = packetFactory->makeDulicateWarningPacket(packet, sender, getNextSequenceNumber());
    interface->send(duplicateWarningPacket, packet->getSender());
}

void AbstractARAClient::updateRoutingTable(Packet* packet, NetworkInterface* interface) {
    // we do not want to send/reinforce routes that would send the packet back over ourselves
    if (isLocalAddress(packet->getPreviousHop()) == false) {
        AddressPtr source = packet->getSource();
        AddressPtr sender = packet->getSender();
        if (routingTable->isNewRoute(source, sender, interface)) {
            createNewRouteFrom(packet, interface);
        }
        else {
            reinforcePheromoneValue(source, sender, interface);
        }

        routingTable->triggerEvaporation();
    }

}

void AbstractARAClient::createNewRouteFrom(Packet* packet, NetworkInterface* interface) {
    if(hasPreviousNodeBeenSeenBefore(packet) == false) {
        float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
        routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue);
    }
}

bool AbstractARAClient::hasPreviousNodeBeenSeenBefore(const Packet* packet) {
    KnownIntermediateHopsMap::const_iterator found = knownIntermediateHops.find(packet->getSource());
    if(found == knownIntermediateHops.end()) {
        // we have never seen any packet for this source address
        return false;
    }

    unordered_set<AddressPtr>* listOfKnownNodes = found->second;

    // have we seen this sender, or the previous hop before?
    return listOfKnownNodes->find(packet->getSender()) != listOfKnownNodes->end()
           || listOfKnownNodes->find(packet->getPreviousHop()) != listOfKnownNodes->end();
}

void AbstractARAClient::handlePacket(Packet* packet, NetworkInterface* interface) {
    if (packet->isDataPacket()) {
        handleDataPacket(packet);
    }
    else if(packet->isAntPacket()) {
        handleAntPacket(packet);
    }
    else if (packet->getType() == PacketType::DUPLICATE_ERROR) {
        handleDuplicateErrorPacket(packet, interface);
    }
    else if (packet->getType() == PacketType::ROUTE_FAILURE) {
        handleRouteFailurePacket(packet, interface);
    }
    else {
        throw Exception("Can not handle packet");
    }
}

void AbstractARAClient::handleDataPacket(Packet* packet) {
    if(isDirectedToThisNode(packet)) {
        deliverToSystem(packet);
    }
    else {
        sendPacket(packet);
    }
}

void AbstractARAClient::handleAntPacket(Packet* packet) {
    if (hasBeenSentByThisNode(packet)) {
        // do not process ant packets we have sent ourselves
        delete packet;
        return;
    }

    if (isDirectedToThisNode(packet)) {
        handleAntPacketForThisNode(packet);
    }
    else if (packet->getTTL() > 0) {
        logTrace("Broadcasting %s %u from %s", PacketType::getAsString(packet->getType()).c_str(), packet->getSequenceNumber(), packet->getSourceString().c_str());
        broadCast(packet);
    }
    else {
        delete packet;
    }
}

void AbstractARAClient::handleAntPacketForThisNode(Packet* packet) {
    char packetType = packet->getType();

    if(packetType == PacketType::FANT) {
        logDebug("FANT %u from %s reached its destination. Broadcasting BANT", packet->getSequenceNumber(), packet->getSourceString().c_str());
        Packet* bant = packetFactory->makeBANT(packet, getNextSequenceNumber());
        broadCast(bant);
    }
    else if(packetType == PacketType::BANT) {
        handleBANTForThisNode(packet);
    }
    else {
        delete packet;
        throw Exception("Can not handle ANT packet (unknown type)");
    }

    delete packet;
}

void AbstractARAClient::handleBANTForThisNode(Packet* bant) {
    AddressPtr routeDiscoveryDestination = bant->getSource();
    if(packetTrap->getNumberOfTrappedPackets(routeDiscoveryDestination) == 0) {
        logWarn("Received BANT %u from %s via %s but there are no trapped packets for this destination.");
    }
    else {
        logInfo("First BANT %u came back from %s via %s. Waiting %ums until delivering the trapped packets", bant->getSequenceNumber(), bant->getSourceString().c_str(), bant->getSenderString().c_str(), packetDeliveryDelayInMilliSeconds);
        stopRouteDiscoveryTimer(routeDiscoveryDestination);
        startDeliveryTimer(routeDiscoveryDestination);
    }
}

void AbstractARAClient::stopRouteDiscoveryTimer(AddressPtr destination) {
    unordered_map<AddressPtr, Timer*>::const_iterator discovery;
    discovery = runningRouteDiscoveries.find(destination);

    if(discovery != runningRouteDiscoveries.end()) {
        Timer* timer = discovery->second;
        timer->interrupt();
        runningRouteDiscoveries.erase(discovery);
        runningRouteDiscoveryTimers.erase(timer);
        delete timer;
    }
}

void AbstractARAClient::startDeliveryTimer(AddressPtr destination) {
    Timer* timer = Environment::getClock()->getNewTimer();
    timer->addTimeoutListener(this);
    timer->run(packetDeliveryDelayInMilliSeconds * 1000);
    runningDeliveryTimers[timer] = destination;
}

void AbstractARAClient::sendDeliverablePackets(AddressPtr destination) {
    deque<Packet*>* deliverablePackets = packetTrap->getDeliverablePackets(destination);
    logDebug("Sending %u trapped packet(s) for destination %s", deliverablePackets->size(), destination->toString().c_str());

    for(auto& deliverablePacket : *deliverablePackets) {
        packetTrap->untrapPacket(deliverablePacket); // TODO packets could already be removed from the trap in packetTrap->getDeliverablePackets(..)
        sendPacket(deliverablePacket);
    }
    delete deliverablePackets;
}

void AbstractARAClient::handleDuplicateErrorPacket(Packet* duplicateErrorPacket, NetworkInterface* interface) {
    routingTable->removeEntry(duplicateErrorPacket->getDestination(), duplicateErrorPacket->getSender(), interface);
}

bool AbstractARAClient::isDirectedToThisNode(const Packet* packet) const {
    return isLocalAddress(packet->getDestination());
}

bool AbstractARAClient::isLocalAddress(AddressPtr address) const {
    for(auto& interface: interfaces) {
        if(interface->getLocalAddress()->equals(address)) {
            return true;
        }
    }
    return false;
}

bool AbstractARAClient::hasBeenSentByThisNode(const Packet* packet) const {
    return isLocalAddress(packet->getSource());
}

void AbstractARAClient::broadCast(Packet* packet) {
    for(auto& interface: interfaces) {
        Packet* packetClone = packetFactory->makeClone(packet);
        packetClone->setPreviousHop(packet->getSender());
        packetClone->setSender(interface->getLocalAddress());
        interface->broadcast(packetClone);
    }
    delete packet;
}

unsigned int AbstractARAClient::getNextSequenceNumber() {
    return nextSequenceNumber++;
}

bool AbstractARAClient::hasBeenReceivedEarlier(const Packet* packet) {
    AddressPtr source = packet->getSource();
    unsigned int sequenceNumber = packet->getSequenceNumber();

    LastReceivedPacketsMap::const_iterator receivedPacketSeqNumbersFromSource = lastReceivedPackets.find(source);
    if(receivedPacketSeqNumbersFromSource != lastReceivedPackets.end()) {
        unordered_set<unsigned int>* sequenceNumbers = receivedPacketSeqNumbersFromSource->second;
        unordered_set<unsigned int>::const_iterator got = sequenceNumbers->find(sequenceNumber);
        if(got != sequenceNumbers->end()) {
            return true;
        }
    }
    return false;
}

void AbstractARAClient::registerReceivedPacket(const Packet* packet) {
    AddressPtr source = packet->getSource();
    AddressPtr sender = packet->getSender();
    AddressPtr previousHop = packet->getPreviousHop();

    // first check the lastReceived sequence numbers for this source
    LastReceivedPacketsMap::const_iterator foundPacketSeqNumbersFromSource = lastReceivedPackets.find(source);
    unordered_set<unsigned int>* listOfSequenceNumbers;
    if(foundPacketSeqNumbersFromSource == lastReceivedPackets.end()) {
        // There is no record of any received packet from this source address ~> create new
        listOfSequenceNumbers = new unordered_set<unsigned int>();
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
        lastReceivedPackets[source] = listOfSequenceNumbers;
    }
    else {
        listOfSequenceNumbers = foundPacketSeqNumbersFromSource->second;
        listOfSequenceNumbers->insert(packet->getSequenceNumber());
    }

    // now check the known intermediate hops for this destination
    KnownIntermediateHopsMap::const_iterator foundIntermediateHopsForSource = knownIntermediateHops.find(source);
    unordered_set<AddressPtr>* listOfKnownIntermediateNodes;
    if(foundIntermediateHopsForSource == knownIntermediateHops.end()) {
        // There is no record of any known intermediate node for this source address ~> create new
        listOfKnownIntermediateNodes = new unordered_set<AddressPtr>();
        listOfKnownIntermediateNodes->insert(packet->getSender());
        if(previousHop != nullptr && previousHop->equals(sender) == false) {
            listOfKnownIntermediateNodes->insert(previousHop);
        }
        knownIntermediateHops[source] = listOfKnownIntermediateNodes;
    }
    else {
        listOfKnownIntermediateNodes = foundIntermediateHopsForSource->second;
        listOfKnownIntermediateNodes->insert(sender);
        if(previousHop != nullptr) {
            listOfKnownIntermediateNodes->insert(previousHop);
        }
    }
}

float AbstractARAClient::calculateInitialPheromoneValue(unsigned int ttl) {
    int alpha = 1; // may change in the future implementations
    return alpha * ttl + initialPheromoneValue;
}

void AbstractARAClient::setRoutingTable(RoutingTable* newRoutingTable){
    packetTrap->setRoutingTable(newRoutingTable);

    // delete old routing table
    delete routingTable;

    // set new routing table
    routingTable = newRoutingTable;
}

void AbstractARAClient::setMaxNrOfRouteDiscoveryRetries(int maxNrOfRouteDiscoveryRetries) {
    this->maxNrOfRouteDiscoveryRetries = maxNrOfRouteDiscoveryRetries;
}

void AbstractARAClient::timerHasExpired(Timer* responsibleTimer) {
    DiscoveryTimerInfo::iterator discoveryTimerInfo = runningRouteDiscoveryTimers.find(responsibleTimer);
    if(discoveryTimerInfo != runningRouteDiscoveryTimers.end()) {
        handleExpiredRouteDiscoveryTimer(responsibleTimer, discoveryTimerInfo->second);
    }
    else {
        DeliveryTimerInfo::iterator deliveryTimerInfo = runningDeliveryTimers.find(responsibleTimer);
        if(deliveryTimerInfo != runningDeliveryTimers.end()) {
            handleExpiredDeliveryTimer(responsibleTimer, deliveryTimerInfo->second);
        }
        else {
            // if this happens its a bug in our code
            throw new Exception("AbstractARAClient::timerHasExpired : Could not identify expired timer");
        }
    }
}

void AbstractARAClient::handleExpiredRouteDiscoveryTimer(Timer* routeDiscoveryTimer, RouteDiscoveryInfo discoveryInfo) {
    const char* destinationString = discoveryInfo.originalPacket->getDestinationString().c_str();
    logInfo("Route discovery for destination %s timed out", destinationString);
    if(discoveryInfo.nrOfRetries < maxNrOfRouteDiscoveryRetries) {
        discoveryInfo.nrOfRetries++;
        logInfo("Restarting discovery for destination %s (%u/%u)", destinationString, discoveryInfo.nrOfRetries, maxNrOfRouteDiscoveryRetries);
        runningRouteDiscoveryTimers[routeDiscoveryTimer] = discoveryInfo;
        sendFANT(discoveryInfo.originalPacket->getDestination());
        routeDiscoveryTimer->run(routeDiscoveryTimeoutInMilliSeconds * 1000);
    }
    else {
        // remove the route discovery timer
        AddressPtr destination = discoveryInfo.originalPacket->getDestination();
        runningRouteDiscoveries.erase(destination);
        runningRouteDiscoveryTimers.erase(routeDiscoveryTimer);
        delete routeDiscoveryTimer;

        deque<Packet*> undeliverablePackets = packetTrap->removePacketsForDestination(destination);
        logWarn("Route discovery for destination %s unsuccessful. Dropping %u packet(s)", destinationString, undeliverablePackets.size());
        for(auto& packet: undeliverablePackets) {
            packetNotDeliverable(packet);
        }
    }
}

void AbstractARAClient::handleExpiredDeliveryTimer(Timer* deliveryTimer, AddressPtr destination) {
    sendDeliverablePackets(destination);
    runningDeliveryTimers.erase(deliveryTimer);
    delete deliveryTimer;
}

void AbstractARAClient::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    routingTable->removeEntry(packet->getDestination(), nextHop, interface);

    if (routingTable->isDeliverable(packet)) {
        logWarn("Link over %s is broken. Sending over alternative route", nextHop->toString().c_str());
        sendPacket(packet);
    }
    else {
        logWarn("Link over %s is broken and can not be repaired. Dropping packet %u from %s.", nextHop->toString().c_str(), packet->getSequenceNumber(), packet->getSourceString().c_str());
        handleCompleteRouteFailure(packet);
    }
}

void AbstractARAClient::handleCompleteRouteFailure(Packet* packet) {
    Packet* routeFailurePacket = packetFactory->makeRouteFailurePacket(packet);
    broadCast(routeFailurePacket);
    delete packet;
}

void AbstractARAClient::handleRouteFailurePacket(Packet* packet, NetworkInterface* interface) {
    AddressPtr destination = packet->getDestination();
    AddressPtr sender = packet->getSender();
    routingTable->removeEntry(destination, sender, interface);
    delete packet;
}

} /* namespace ARA */
