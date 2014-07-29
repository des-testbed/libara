/*
 * $FU-Copyright$
 */

#include "AbstractNetworkClient.h"

using namespace std;

ARA_NAMESPACE_BEGIN

AbstractNetworkClient::~AbstractNetworkClient() {
    /* The following members may have be deleted earlier, depending on the destructor of the implementing class */
    DELETE_IF_NOT_NULL(logger);
    DELETE_IF_NOT_NULL(packetTrap);
    DELETE_IF_NOT_NULL(routingTable);
    DELETE_IF_NOT_NULL(packetFactory);
}

void AbstractNetworkClient::setRoutingTable(RoutingTable* newRoutingTable){
    packetTrap->setRoutingTable(newRoutingTable);

    // delete old routing table
    delete routingTable;

    // set new routing table
    routingTable = newRoutingTable;
}

void AbstractNetworkClient::setLogger(Logger* logger) {
    this->logger = logger;
}

void AbstractNetworkClient::logTrace(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_TRACE, args);
        va_end(args);
    }
}

void AbstractNetworkClient::logDebug(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_DEBUG, args);
        va_end(args);
    }
}

void AbstractNetworkClient::logInfo(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_INFO, args);
        va_end(args);
    }
}

void AbstractNetworkClient::logWarn(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_WARN, args);
        va_end(args);
    }
}

void AbstractNetworkClient::logError(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_ERROR, args);
        va_end(args);
    }
}

void AbstractNetworkClient::logFatal(const std::string &text, ...) const {
    if(logger != nullptr) {
        va_list args;
        va_start(args, text);
        logger->logMessageWithVAList(text, Logger::LEVEL_FATAL, args);
        va_end(args);
    }
}

void AbstractNetworkClient::addNetworkInterface(NetworkInterface* newInterface) {
    interfaces.push_back(newInterface);
}

NetworkInterface* AbstractNetworkClient::getNetworkInterface(unsigned int index) {
    return interfaces.at(index);
}

unsigned int AbstractNetworkClient::getNumberOfNetworkInterfaces() {
    return interfaces.size();
}

PacketFactory* AbstractNetworkClient::getPacketFactory() const{
    return packetFactory;
}

bool AbstractNetworkClient::isDirectedToThisNode(const Packet* packet) const {
    return isLocalAddress(packet->getDestination());
}

bool AbstractNetworkClient::hasBeenSentByThisNode(const Packet* packet) const {
    return isLocalAddress(packet->getSource());
}

bool AbstractNetworkClient::isLocalAddress(AddressPtr address) const {
    for(auto& interface: interfaces) {
        if(interface->getLocalAddress()->equals(address)) {
            return true;
        }
    }
    return false;
}

void AbstractNetworkClient::broadCast(Packet* packet) {
    for(auto& interface: interfaces) {
        Packet* packetClone = packetFactory->makeClone(packet);
        packetClone->setPreviousHop(packet->getSender());
        packetClone->setSender(interface->getLocalAddress());
        interface->broadcast(packetClone);
    }
    delete packet;
}

unsigned int AbstractNetworkClient::getNextSequenceNumber() {
    return nextSequenceNumber++;
}

ARA_NAMESPACE_END
