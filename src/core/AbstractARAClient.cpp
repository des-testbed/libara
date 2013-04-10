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

AbstractARAClient::AbstractARAClient(Configuration& configuration, RoutingTable *routingTable) {
    initialize(configuration, routingTable);
}

void AbstractARAClient::initialize(Configuration& configuration, RoutingTable *routingTable) {
    forwardingPolicy = configuration.getForwardingPolicy();
    pathReinforcementPolicy = configuration.getReinforcementPolicy();
    evaporationPolicy = configuration.getEvaporationPolicy();
    initialPheromoneValue = configuration.getInitialPheromoneValue();
    maxNrOfRouteDiscoveryRetries = configuration.getMaxNrOfRouteDiscoveryRetries();
    maxHopCount = configuration.getMaxTTL();
    routeDiscoveryTimeoutInMilliSeconds = configuration.getRouteDiscoveryTimeoutInMilliSeconds();

    this->routingTable = routingTable;
    routingTable->setEvaporationPolicy(evaporationPolicy);

    packetTrap = new PacketTrap(routingTable);
    runningRouteDiscoveries = unordered_map<AddressPtr, Timer*>();
    runningRouteDiscoveryTimers = unordered_map<Timer*, RouteDiscoveryInfo>();

    packetFactory = Environment::getPacketFactory();
}

AbstractARAClient::~AbstractARAClient() {
    // delete logger if it has been set
    if(logger != nullptr) {
        delete logger;
    }

    // delete the sequence number lists of the last received packets
    unordered_map<AddressPtr, unordered_set<unsigned int>*>::iterator iterator1;
    for (iterator1=lastReceivedPackets.begin(); iterator1!=lastReceivedPackets.end(); iterator1++) {
        // the addresses are disposed of automatically by shared_ptr
        delete iterator1->second;
    }
    lastReceivedPackets.clear();

    // delete running route discovery timers
    unordered_map<Timer*, RouteDiscoveryInfo>::iterator iterator2;
    for (iterator2=runningRouteDiscoveryTimers.begin(); iterator2!=runningRouteDiscoveryTimers.end(); iterator2++) {
        delete iterator2->first;
    }
    runningRouteDiscoveryTimers.clear();

    delete packetTrap;
    delete routingTable;
    delete pathReinforcementPolicy;
    delete evaporationPolicy;
    delete forwardingPolicy;
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

void AbstractARAClient::sendPacket(Packet* packet) {
    if(packet->getTTL() > 0) {
        if(routingTable->isDeliverable(packet)) {
            NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
            NetworkInterface* interface = nextHop->getInterface();
            AddressPtr nextHopAddress = nextHop->getAddress();
            packet->setSender(interface->getLocalAddress());

            logTrace("Forwarding DATA packet %u from %s to %s via %s", packet->getSequenceNumber(), packet->getSourceString(), packet->getDestinationString(), nextHopAddress->toString());
            reinforcePheromoneValue(packet->getDestination(), nextHopAddress, interface);

            interface->send(packet, nextHopAddress);
        } else {
            logDebug("Packet %u from %s to %s is not deliverable. Starting route discovery phase", packet->getSequenceNumber(), packet->getSourceString(), packet->getDestinationString());
            packetTrap->trapPacket(packet);

            unsigned int sequenceNr = getNextSequenceNumber();
            Packet* fant = packetFactory->makeFANT(packet, sequenceNr, maxHopCount);
            broadCast(fant);

            startRouteDiscoveryTimer(packet);
        }
    }
    else {
        delete packet;
    }
}

void AbstractARAClient::reinforcePheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    float currentPheromoneValue = routingTable->getPheromoneValue(destination, nextHop, interface);
    float newPheromoneValue = pathReinforcementPolicy->calculateReinforcedValue(currentPheromoneValue);
    routingTable->update(destination, nextHop, interface, newPheromoneValue);
}

void AbstractARAClient::startRouteDiscoveryTimer(const Packet* packet) {
        AddressPtr destination = packet->getDestination();
    if (isRouteDiscoveryRunning(destination) == false) {
        Timer* timer = Environment::getClock()->getNewTimer();
        timer->addTimeoutListener(this);
        timer->run(routeDiscoveryTimeoutInMilliSeconds * 1000);

        RouteDiscoveryInfo discoveryInfo;
        discoveryInfo.nrOfRetries = 0;
        discoveryInfo.timer = timer;
        discoveryInfo.originalPacket = packet;

        runningRouteDiscoveries[destination] = timer;
        runningRouteDiscoveryTimers[timer] = discoveryInfo;
    }
}

bool AbstractARAClient::isRouteDiscoveryRunning(AddressPtr destination) {
    return runningRouteDiscoveries.find(destination) != runningRouteDiscoveries.end();
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
    delete packet;
}

void AbstractARAClient::sendDuplicateWarning(Packet* packet, NetworkInterface* interface) {
    AddressPtr sender = interface->getLocalAddress();
    Packet* duplicateWarningPacket = packetFactory->makeDulicateWarningPacket(packet, sender, getNextSequenceNumber());
    interface->send(duplicateWarningPacket, packet->getSender());
}

void AbstractARAClient::updateRoutingTable(Packet* packet, NetworkInterface* interface) {
    if (hasBeenSentByThisNode(packet) == false) {
        AddressPtr source = packet->getSource();
        AddressPtr destination = packet->getDestination();
        AddressPtr sender = packet->getSender();
        if (routingTable->isNewRoute(source, sender, interface)) {
            float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
            routingTable->update(source, sender, interface, initialPheromoneValue);
        }
        else {
            reinforcePheromoneValue(source, packet->getSender(), interface);
        }
    }
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
        logTrace("Broadcasting %s %u from %s", PacketType::getAsString(packet->getType()).c_str(), packet->getSequenceNumber(), packet->getSourceString());
        broadCast(packet);
    }
    else {
        delete packet;
    }
}

void AbstractARAClient::handleAntPacketForThisNode(Packet* packet) {
    char packetType = packet->getType();

    if(packetType == PacketType::FANT) {
        logDebug("FANT %u from %s reached its destination. Broadcasting BANT", packet->getSequenceNumber(), packet->getSourceString());
        Packet* bant = packetFactory->makeBANT(packet, getNextSequenceNumber(), maxHopCount);
        broadCast(bant);
    }
    else if(packetType == PacketType::BANT) {
        stopRouteDiscoveryTimer(packet->getSource());
        sendDeliverablePackets(packet);
    }
    else {
        delete packet;
        throw Exception("Can not handle ANT packet (unknown type)");
    }

    delete packet;
}

void AbstractARAClient::stopRouteDiscoveryTimer(AddressPtr destination) {
    unordered_map<AddressPtr, Timer*>::const_iterator discovery;
    discovery = runningRouteDiscoveries.find(destination);

    if(discovery != runningRouteDiscoveries.end()) {
        Timer* timer = discovery->second;
        timer->interrupt();
        runningRouteDiscoveries.erase(discovery);
        delete timer;
    }
}

void AbstractARAClient::sendDeliverablePackets(const Packet* packet) {
    deque<Packet*>* deliverablePackets = packetTrap->getDeliverablePackets();
    logDebug("BANT %u came back from %s. %u trapped packet can now be delivered", packet->getSequenceNumber(), packet->getSourceString(), deliverablePackets->size());

    for(auto& deliverablePacket : *deliverablePackets) {
        packetTrap->untrapPacket(deliverablePacket); //TODO We want to remove the packet from the trap only if we got an acknowledgment back
        sendPacket(deliverablePacket);
    }
    delete deliverablePackets;
}

void AbstractARAClient::handleDuplicateErrorPacket(Packet* duplicateErrorPacket, NetworkInterface* interface) {
    routingTable->removeEntry(duplicateErrorPacket->getDestination(), duplicateErrorPacket->getSender(), interface);
}

bool AbstractARAClient::isDirectedToThisNode(const Packet* packet) const {
    AddressPtr destination = packet->getDestination();
    for(auto& interface: interfaces) {
        if(interface->getLocalAddress()->equals(destination)) {
            return true;
        }
    }
    return false;
}

bool AbstractARAClient::hasBeenSentByThisNode(const Packet* packet) const {
    AddressPtr source = packet->getSource();
    for(auto& interface: interfaces) {
        if(interface->getLocalAddress()->equals(source)) {
            return true;
        }
    }
    return false;
}

void AbstractARAClient::broadCast(Packet* packet) {
    for(auto& interface: interfaces) {
        Packet* packetClone = packetFactory->makeClone(packet);
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

    unordered_map<AddressPtr, unordered_set<unsigned int>*>::const_iterator receivedPacketSeqNumbersFromSource = lastReceivedPackets.find(source);
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
    unordered_map<AddressPtr, unordered_set<unsigned int>*>::const_iterator foundPacketSeqNumbersFromSource = lastReceivedPackets.find(source);

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

void AbstractARAClient::timerHasExpired(Timer* routeDiscoveryTimer) {
    unordered_map<Timer*, RouteDiscoveryInfo>::const_iterator discovery;
    discovery = runningRouteDiscoveryTimers.find(routeDiscoveryTimer);

    if(discovery == runningRouteDiscoveryTimers.end()) {
        // if this happens its a bug in our code
        throw new Exception("AbstractARAClient::timerHasExpired : Could not find running route discovery timer");
    }

    RouteDiscoveryInfo discoveryInfo = discovery->second;
    if(discoveryInfo.nrOfRetries < maxNrOfRouteDiscoveryRetries) {
        discoveryInfo.nrOfRetries++;
        runningRouteDiscoveryTimers[routeDiscoveryTimer] = discoveryInfo;
        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* fant = packetFactory->makeFANT(discoveryInfo.originalPacket, sequenceNr, maxHopCount);
        broadCast(fant);
        routeDiscoveryTimer->run(routeDiscoveryTimeoutInMilliSeconds * 1000);
    }
    else {
        // remove the route discovery timer
        AddressPtr destination = discoveryInfo.originalPacket->getDestination();
        runningRouteDiscoveries.erase(destination);
        runningRouteDiscoveryTimers.erase(routeDiscoveryTimer);
        delete routeDiscoveryTimer;

        deque<Packet*> undeliverablePackets = packetTrap->removePacketsForDestination(destination);
        for(auto& packet: undeliverablePackets) {
            packetNotDeliverable(packet);
        }
    }
}

void AbstractARAClient::handleRouteFailure(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    AddressPtr destination = packet->getDestination();

    routingTable->removeEntry(destination, nextHop, interface);

    if (routingTable->isDeliverable(destination)) {
        sendPacket(packet);
    }
    else {
        Packet* routeFailurePacket = packetFactory->makeRouteFailurePacket(packet);
        broadCast(routeFailurePacket);
        delete packet;
    }
}

void AbstractARAClient::handleRouteFailurePacket(Packet* packet, NetworkInterface* interface) {
    AddressPtr destination = packet->getDestination();
    AddressPtr sender = packet->getSender();
    routingTable->removeEntry(destination, sender, interface);
    delete packet;
}

} /* namespace ARA */
