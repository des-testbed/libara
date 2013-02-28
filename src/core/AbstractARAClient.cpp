/*
 * $FU-Copyright$
 */

#include "AbstractARAClient.h"
#include "PacketType.h"
#include "LinearPathReinforcementPolicy.h"

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

AbstractARAClient::AbstractARAClient(TimeFactory* timeFactory) {
    routingTable = new RoutingTable(timeFactory);
    packetTrap = new PacketTrap(routingTable);
    /// set it to a 'random' initial value FIXME: WHY?
    this->initialPhi = 1.0;
    this->deltaPhi = 2.0;
    this->pathReinforcementPolicy = new LinearPathReinforcementPolicy(this->routingTable, deltaPhi); //FIXME this needs to be more flexible (pure virtual getter)
}

AbstractARAClient::~AbstractARAClient() {
    delete packetTrap;

    // delete logger if it has been set
    if(logger != nullptr) {
        delete logger;
    }

    // delete the sequence number lists of the last received packets
    unordered_map<AddressPtr, unordered_set<unsigned int>*>::iterator iterator;
    for (iterator=lastReceivedPackets.begin(); iterator!=lastReceivedPackets.end(); iterator++) {
        pair<AddressPtr, unordered_set<unsigned int>*> entryPair = *iterator;

        // the addresses are disposed of automatically by shared_ptr
        delete entryPair.second;
    }
    lastReceivedPackets.clear();

    delete routingTable;
    delete pathReinforcementPolicy;
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

void AbstractARAClient::sendPacket(const Packet* packet) {
    if(routingTable->isDeliverable(packet)) {
        NextHop* nextHop = getNextHop(packet);
        NetworkInterface* interface = nextHop->getInterface();
        AddressPtr nextHopAddress = nextHop->getAddress();
        Packet* newPacket = packet->clone();
        newPacket->setSender(interface->getLocalAddress());
        newPacket->increaseHopCount();

        logTrace("Forwarding DATA packet %u from %s to %s via %s", packet->getSequenceNumber(), packet->getSenderString(), packet->getDestinationString(), nextHopAddress->toString());
        pathReinforcementPolicy->update(packet->getDestination(), nextHopAddress, interface);
        interface->send(newPacket, nextHop->getAddress());
        delete newPacket;
    } else {
        logDebug("Packet %u from %s to %s is not deliverable. Starting route discovery phase", packet->getSequenceNumber(), packet->getSourceString(), packet->getDestinationString());
        packetTrap->trapPacket(packet);
        unsigned int sequenceNr = getNextSequenceNumber();
        Packet* fant = packet->createFANT(sequenceNr);
        broadCast(fant);
        delete fant;
    }
}

void AbstractARAClient::receivePacket(const Packet* packet, NetworkInterface* interface) {
    ///
    updateRoutingTable(packet, interface);  //FIXME Check if it is ok to update the routing table here

    if(hasBeenReceivedEarlier(packet)) {
        if(packet->isDataPacket()) {
            sendDuplicateWarning(packet, interface);
        }
        return;
    } else {
        registerReceivedPacket(packet);
    }

    handlePacket(packet, interface);
}

NextHop* AbstractARAClient::getNextHop(const Packet* packet) {
    ForwardingPolicy* forwardingPolicy = getForwardingPolicy();
    return forwardingPolicy->getNextHop(packet);
}

void AbstractARAClient::sendDuplicateWarning(const Packet* packet, NetworkInterface* interface) {
    Packet* duplicateWarningPacket = packet->createDuplicateWarning();
    duplicateWarningPacket->setSender(interface->getLocalAddress());
    interface->send(duplicateWarningPacket, packet->getSender());
    delete duplicateWarningPacket;
}

void AbstractARAClient::handlePacket(const Packet* packet, NetworkInterface* interface) {
    if(packet->isDataPacket()){
        handleDataPacket(packet);
    }else if(packet->isAntPacket()){
       /// only add the entry if does not exist (otherwise the phi value of the already existing would be reset)
        if(!(routingTable->isDeliverable(packet))){
           float phi = this->initializePheromone(packet);
           this->routingTable->update(packet->getSource(), packet->getSender(), interface, phi);
        }
        ///
        handleAntPacket(packet);
    }
    else if(packet->getType() == PacketType::DUPLICATE_ERROR) {
        handleDuplicateErrorPacket(packet, interface);
    }
    // TODO throw exception if we can not handle this packet
}

void AbstractARAClient::handleDataPacket(const Packet* packet) {
    if(isDirectedToThisNode(packet)) {
        deliverToSystem(packet);
    }
    else {
        sendPacket(packet);
    }
}

void AbstractARAClient::handleAntPacket(const Packet* packet) {
    if(hasBeenSentByThisNode(packet) == false) {
        
        if(isDirectedToThisNode(packet) == false) {
            logTrace("Broadcasting %s %u from %s", PacketType::getAsString(packet->getType()).c_str(), packet->getSequenceNumber(), packet->getSourceString());
            broadCast(packet);
        }
        else {
            handleAntPacketForThisNode(packet);
        }
    }
}

void AbstractARAClient::handleAntPacketForThisNode(const Packet* packet) {
    char packetType = packet->getType();

    if(packetType == PacketType::FANT) {
        logDebug("FANT %u from %s reached its destination. Broadcasting BANT", packet->getSequenceNumber(), packet->getSourceString());
        Packet* bant = packet->createBANT(getNextSequenceNumber());
        broadCast(bant);
        delete bant;
    }
    else if(packetType == PacketType::BANT) {
        deque<const Packet*>* deliverablePackets = packetTrap->getDeliverablePackets();
        logDebug("BANT %u came back from %s. %u trapped packet can now be delivered", packet->getSequenceNumber(), packet->getSourceString(), deliverablePackets->size());
        for(auto& deliverablePacket : *deliverablePackets) {
            sendPacket(deliverablePacket);
            packetTrap->untrapPacket(deliverablePacket); //TODO We want to remove the packet from the trap only if we got an acknowledgment back
        }
        delete deliverablePackets;
    }
    else {
        // TODO throw exception if we can not handle this packet
    }
}

void AbstractARAClient::handleDuplicateErrorPacket(const Packet* packet, NetworkInterface* interface) {
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

void AbstractARAClient::broadCast(const Packet* packet) {
    Packet* newPacket = packet->clone();
    newPacket->increaseHopCount();

    for(auto& interface: interfaces) {
        newPacket->setSender(interface->getLocalAddress());
        interface->broadcast(newPacket);
    }

    delete newPacket;
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

/**
 * The method determines the initial pheromone value of a new routing table
 * entry. The pheromone value is only determined for a non-existing routing
 * table entry.
 *
 * @param in packet The received packet 
 *
 * @return The pheromone value
 */
float AbstractARAClient::initializePheromone(const Packet* packet){
    /// determine the hop count malus
    float hopCountMalus = 1 / (float) packet->getHopCount();
    /// compute the phi value   
    return (this->initialPhi * hopCountMalus);
}

void AbstractARAClient::setRoutingTable(RoutingTable *routingTable){
    // update packet trap to new routing table
    this->packetTrap->setRoutingTable(routingTable);
    // delete old routing table
    delete this->routingTable;
    // set new routing table
    this->routingTable = routingTable;
}
} /* namespace ARA */
