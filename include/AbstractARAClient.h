/*
 * $FU-Copyright$
 */

#ifndef ABSTRACTARACLIENT_H_
#define ABSTRACTARACLIENT_H_

#include "Logger.h"
#include "Address.h"
#include "NextHop.h"
#include "NetworkInterface.h"
#include "PacketTrap.h"
#include "Clock.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "ForwardingPolicy.h"
#include "PathReinforcementPolicy.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <memory>
#include <string>
#include <sstream>

namespace ARA {

//TODO fix the visibility: most of the methods should be protected instead of public
//TODO fix the indent

/**
 * TODO write class description
 */
class AbstractARAClient {

public:
    AbstractARAClient(Clock* clock);
    virtual ~AbstractARAClient();

    /**
     * Sends the packet to the packets destination.
     *
     * If the packet is deliverable (e.g there is at least one route known
     * in the routing table), the next hop is calculated via the current
     * forwarding policy. If there is no known route to the packet destination
     * a FANT is generated and send according to the Abstract Ant Routing
     * Algorithm (ARA).
     *
     * @param packet the Packet to be send. Please note that the packet will be
     *        deleted by the AbstractARAClient when it is no longer needed.
     */
    void sendPacket(Packet* packet);

    /**
     * Receive a Packet over the given NetworkInterface. The packet will be
     * processed according to the Ant Routing Algorithm (ARA).
     *
     * @param packet the received Packet. Please note that the packet will be
     *        deleted by the AbstractARAClient when it has been fully processed.
     */
    virtual void receivePacket(Packet* packet, NetworkInterface* interface);

    /**
     * This method is called each a time a new packet is received over the
     * given interface.
     *
     * It is responsible for updating the routing table so a route to the
     * packets source is known in the future.
     *
     * Note: This method is not called on duplicate packets (which trigger a DUPLICATE_ERROR).
     */
    virtual void updateRoutingTable(const Packet* packet, NetworkInterface* interface) = 0;

    /**
     * The packet should be directed to this node and must be delivered to the local system.
     * Please note that this method is responsible for deleting the given packet (or delegating
     * this responsibility to another method)
     */
    virtual void deliverToSystem(const Packet* packet) = 0;

    virtual void packetIsNotDeliverable(const Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface) = 0;

    /**
     * Sets a logger for this ARA client.
     *
     * This logger will be used to log messages during the routing algorithm.
     * It will be deleted in the destructor of this client.
     */
    void setLogger(Logger* logger);

    /**
     * Registers a new NetworkInterface at this client.
     */
    void addNetworkInterface(NetworkInterface* newInterface);

    /**
     * Returns a specific NetworkInterface by index.
     */
    NetworkInterface* getNetworkInterface(unsigned int index);

    /**
     * Returns the amount of registered network interfaces of this client.
     */
    unsigned int getNumberOfNetworkInterfaces();

    //TODO AbstractARAClient::broadCast(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    void broadCast(Packet* packet);
    //TODO AbstractARAClient::getNextSequenceNumber(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    unsigned int getNextSequenceNumber();
    //TODO AbstractARAClient::hasBeenReceivedEarlier(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    bool hasBeenReceivedEarlier(const Packet* packet);
    //TODO AbstractARAClient::registerReceivedPacket(...) should be private. It is not because else the AbstractARAClientTest can not see this.. :(
    void registerReceivedPacket(const Packet* packet);

    /// The computes the initial pheromone value of a link
    virtual float initializePheromone(const Packet* packet);
    ///
    virtual void setEvaporationPolicy(EvaporationPolicy *policy) = 0;

    void setRoutingTable(RoutingTable *routingTable);

protected:
    /**
     * This method is called to retrieve an instance of ForwardingPolicy
     * each time the next hop for a given destination has to be determined.
     *
     * Note: If the forwarding policy is static (i.e. does never change), the
     * implementation should store the forwarding policy as a member and just
     * return a pointer to it instead of creating a new instance each time
     * this method is called.
     */
    virtual ForwardingPolicy* getForwardingPolicy() = 0;

    /**
     * Checks if a logger has been assigned to this ARA client and if so
     * delegates the call to it with Logger::LEVEL_TRACE.
     *
     * The optional varargs are handled as parameters to the logMessage string.
     * If no logger has been set via setLogger() nothing happens.
     *
     * @see AbstractARAClient::logTrace
     * @see AbstractARAClient::logDebug
     * @see AbstractARAClient::logInfo
     * @see AbstractARAClient::logWarn
     * @see AbstractARAClient::logError
     * @see AbstractARAClient::logFatal
     */
    void logMessage(const std::string &logMessage, Logger::Level level, ...) const;

    /**
     * Logs with trace level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logTrace(const std::string &logMessage, ...) const;

    /**
     * Logs with debug level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logDebug(const std::string &logMessage, ...) const;

    /**
     * Logs with info level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logInfo(const std::string &logMessage, ...) const;

    /**
     * Logs with warn level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logWarn(const std::string &logMessage, ...) const;

    /**
     * Logs with error level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logError(const std::string &logMessage, ...) const;

    /**
     * Logs with fatal level.
     *
     * @see AbstractARAClient::logMessage
     */
    void logFatal(const std::string &logMessage, ...) const;

private:
    NextHop* getNextHop(const Packet* packet);
    void handleDuplicatePacket(Packet* packet, NetworkInterface* interface);
    void sendDuplicateWarning(Packet* packet, NetworkInterface* interface);
    void handlePacket(Packet* packet, NetworkInterface* interface);
    void handleDataPacket(Packet* packet);
    void handleAntPacket(Packet* packet);
    void handleAntPacketForThisNode(Packet* packet);
    void handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface);
    bool isDirectedToThisNode(const Packet* packet) const;
    bool hasBeenSentByThisNode(const Packet* packet) const;

protected:
    /// The member denotes the constant which is used in the pheromone reinforcement of a path
    // FIXME do we need this here any more? I thought we have a policy class for that
    double deltaPhi;

    std::deque<NetworkInterface*> interfaces;
    RoutingTable *routingTable;
    PacketTrap* packetTrap;

    // FIXME let the AbstractARAClient access this object via a pure virtual method to force the implementations to actually set this policy
    PathReinforcementPolicy* pathReinforcementPolicy;

    /// The member specifies the initial level
    // FIXME do we need this here any more? I thought we have a policy class for that
    double initialPhi;

private:
    Logger* logger = nullptr;
    unsigned int nextSequenceNumber = 1;
    std::unordered_map<std::shared_ptr<Address>, std::unordered_set<unsigned int>*, AddressHash, AddressPredicate> lastReceivedPackets;
};

} /* namespace ARA */
#endif /* ABSTRACTARACLIENT_H_ */
