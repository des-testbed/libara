/*
 * $FU-Copyright$
 */

#include "AbstractARAClient.h"
#include "PacketType.h"
#include "LinearPathReinforcementPolicy.h"
#include "Timer.h"
#include "Environment.h"
#include "Exception.h"

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
    routeDiscoveryTimeoutInMilliSeconds = configuration.getRouteDiscoveryTimeoutInMilliSeconds();

    this->routingTable = routingTable;
    routingTable->setEvaporationPolicy(evaporationPolicy);

    packetTrap = new PacketTrap(routingTable);
    runningRouteDiscoveries = unordered_map<AddressPtr, Timer*>();
    runningRouteDiscoveryTimers = unordered_map<Timer*, RouteDiscoveryInfo>();
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
    if(routingTable->isDeliverable(packet)) {
        NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
        NetworkInterface* interface = nextHop->getInterface();
        AddressPtr nextHopAddress = nextHop->getAddress();
        packet->setSender(interface->getLocalAddress());
        packet->increaseHopCount();

        logTrace("Forwarding DATA packet %u from %s to %s via %s", packet->getSequenceNumber(), packet->getSourceString(), packet->getDestinationString(), nextHopAddress->toString());
        reinforcePheromoneValue(packet->getDestination(), nextHopAddress, interface);

        interface->send(packet, nextHopAddress);
    } else {
        logDebug("Packet %u from %s to %s is not deliverable. Starting route discovery phase", packet->getSequenceNumber(), packet->getSourceString(), packet->getDestinationString());
        packetTrap->trapPacket(packet);

        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* fant = packet->createFANT(sequenceNr);
        broadCast(fant);

        startRouteDiscoveryTimer(packet);
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
    Packet* duplicateWarningPacket = packet->createDuplicateWarning();
    duplicateWarningPacket->setSender(interface->getLocalAddress());
    interface->send(duplicateWarningPacket, packet->getSender());
}

void AbstractARAClient::handlePacket(Packet* packet, NetworkInterface* interface) {
    if (hasBeenSentByThisNode(packet) == false) {
        // do not insert values to self in the routing table
        AddressPtr source = packet->getSource();
        if (routingTable->isDeliverable(source)) {
            // update an existing pheromone value
            reinforcePheromoneValue(source, packet->getSender(), interface);
        }
        else {
            // initialize a new pheromone value
            float initialPheromoneValue = calculateInitialPheromoneValue(packet->getHopCount());
            routingTable->update(source, packet->getSender(), interface, initialPheromoneValue);
        }
    }

    if (packet->isDataPacket()) {
        handleDataPacket(packet);
    }
    else if(packet->isAntPacket()) {
        handleAntPacket(packet);
    }
    else if (packet->getType() == PacketType::DUPLICATE_ERROR) {
        handleDuplicateErrorPacket(packet, interface);
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
    if(hasBeenSentByThisNode(packet) == false) {

        if(isDirectedToThisNode(packet) == false) {
            logTrace("Broadcasting %s %u from %s", PacketType::getAsString(packet->getType()).c_str(), packet->getSequenceNumber(), packet->getSourceString());
            broadCast(packet);
        }
        else {
            handleAntPacketForThisNode(packet);
        }
    }
    else {
        delete packet;
    }
}

void AbstractARAClient::handleAntPacketForThisNode(Packet* packet) {
    char packetType = packet->getType();

    if(packetType == PacketType::FANT) {
        logDebug("FANT %u from %s reached its destination. Broadcasting BANT", packet->getSequenceNumber(), packet->getSourceString());
        Packet* bant = packet->createBANT(getNextSequenceNumber());
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

void AbstractARAClient::handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface) {
    routingTable->removeEntry(packet->getDestination(), packet->getSender(), interface);
    // TODO we can also invalidate the ack timer for the packet
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
    packet->increaseHopCount();

    for(auto& interface: interfaces) {
        Packet* packetClone = packet->clone();
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

float AbstractARAClient::calculateInitialPheromoneValue(unsigned int hopCount) {
    float hopCountMalus = 1 / (float) hopCount;
    return initialPheromoneValue * hopCountMalus;
}

void AbstractARAClient::setRoutingTable(RoutingTable *routingTable){
    // update packet trap to new routing table
    this->packetTrap->setRoutingTable(routingTable);
    // delete old routing table
    delete this->routingTable;
    // set new routing table
    this->routingTable = routingTable;
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
        Packet* fant = discoveryInfo.originalPacket->createFANT(sequenceNr);
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

} /* namespace ARA */
