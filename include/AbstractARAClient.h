/*
 * $FU-Copyright$
 */

#ifndef ABSTRACTARACLIENT_H_
#define ABSTRACTARACLIENT_H_

#include "TimeoutEventListener.h"
#include "Configuration.h"
#include "Logger.h"
#include "Address.h"
#include "NextHop.h"
#include "NetworkInterface.h"
#include "PacketTrap.h"
#include "PacketFactory.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "ForwardingPolicy.h"
#include "PathReinforcementPolicy.h"
#include "RouteDiscoveryInfo.h"
#include "Timer.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <memory>
#include <string>

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

//TODO fix the visibility: most of the methods should be protected instead of public
//TODO fix the indent

/**
 * TODO write class description
 */
class AbstractARAClient : public TimeoutEventListener {

public:
    /**
     * This standard constructor is only provided for those concrete implementations that
     * absolutely need a constructor without parameters (like in OMNeT++). If you use this
     * constructor you must make sure to call AbstractARAClient::initialize before any call
     * to AbstractARAClient::sendPacket or AbstractARAClient::receivePacket.
     *
     * The recommended way is the constructor that accepts a Configuration object which will
     * handle initialization by default.
     */
    AbstractARAClient() {}

    /**
     * This is the recommended constructor which should be used by all concrete implementation
     * if possible. It will initialize the client with the given configuration so no additional
     * call to AbstractARAClient::initialize is required.
     */
    AbstractARAClient(Configuration& configuration, RoutingTable *routingTable, PacketFactory* packetFactory);

    /**
     * The standard virtual destructor of this abstract class.
     */
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
     * This method is called each time packet can not be delivered to a specific next hop address.
     * This is the case if this client never receives an acknowledgment in the timeout period
     * and has tried too many times.
     */
    virtual void handleRouteFailure(Packet* packet, AddressPtr nextHop, NetworkInterface* interface);

    /**
     * This method will initialize this client with the given configuration, ~RoutingTable and ~PacketFactory.
     * It must be called before any call to AbstractARAClient::sendPacket or
     * AbstractARAClient::receivePacket. If this object has been created by the
     * standard constructor this method must be called manually.
     */
    void initialize(Configuration& configuration, RoutingTable *routingTable, PacketFactory* packetFactory);

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

    PacketFactory* getPacketFactory() const;

    //TODO AbstractARAClient::broadCast(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    void broadCast(Packet* packet);
    //TODO AbstractARAClient::getNextSequenceNumber(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    unsigned int getNextSequenceNumber();
    //TODO AbstractARAClient::hasBeenReceivedEarlier(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    bool hasBeenReceivedEarlier(const Packet* packet);
    //TODO AbstractARAClient::registerReceivedPacket(...) should be private. It is not because else the AbstractARAClientTest can not see this.. :(
    void registerReceivedPacket(const Packet* packet);

    void setRoutingTable(RoutingTable *routingTable);

    virtual void timerHasExpired(Timer* responsibleTimer);

    void setMaxNrOfRouteDiscoveryRetries(int maxNrOfRouteDiscoveryRetries);

protected:
    /**
     * The packet should be directed to this node and must be delivered to the local system.
     * Please note that this method is responsible for deleting the given packet (or delegating
     * this responsibility to another method)
     */
    virtual void deliverToSystem(const Packet* packet) = 0;

    /**
     * This method is called if the route discovery is unsuccessful and not route to the packets
     * destination can be established. The task of this method is to notify the upper layers
     * about this event and delete the packet.
     */
    virtual void packetNotDeliverable(const Packet* packet) = 0;

    /**
     * This method either initializes or reinforces a route in the routing table.
     */
    void updateRoutingTable(Packet* packet, NetworkInterface* interface);

    /**
     * Calculates an initial pheromone value based on the initialPhi value from the Configuration
     * and the given TTL with the formula with alpha=1.
     * result = alpha * TTL + initialPheromoneValue
     */
    float calculateInitialPheromoneValue(unsigned int ttl);

    /**
     * Handles path reinforcement using the currently set PathReinforcementPolicy.
     */
    void reinforcePheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

    /**
     * Handles a packet depending on its type. This method is protected virtual to enable
     * other ARA implementations (like EARA) to override it to add new packet types.
     */
    virtual void handlePacket(Packet* packet, NetworkInterface* interface);

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

    void handleDuplicatePacket(Packet* packet, NetworkInterface* interface);
    void sendDuplicateWarning(Packet* packet, NetworkInterface* interface);
    void handleDataPacket(Packet* packet);
    void handleAntPacket(Packet* packet);
    void handleAntPacketForThisNode(Packet* packet);
    virtual void handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface);
    void handleRouteFailurePacket(Packet* packet, NetworkInterface* interface);
    bool isDirectedToThisNode(const Packet* packet) const;
    bool hasBeenSentByThisNode(const Packet* packet) const;
    void startNewRouteDiscovery(const Packet* packet);
    void startRouteDiscoveryTimer(const Packet* packet);
    void sendFANT(AddressPtr destination);
    bool isRouteDiscoveryRunning(AddressPtr destination);
    void stopRouteDiscoveryTimer(AddressPtr destination);
    void sendDeliverablePackets(const Packet* packet);

protected:
    std::unordered_map<AddressPtr, Timer*> runningRouteDiscoveries;
    std::unordered_map<Timer*, RouteDiscoveryInfo> runningRouteDiscoveryTimers;

    ForwardingPolicy* forwardingPolicy;
    PathReinforcementPolicy* pathReinforcementPolicy;
    EvaporationPolicy* evaporationPolicy;

    std::deque<NetworkInterface*> interfaces;
    RoutingTable* routingTable;
    PacketTrap* packetTrap;
    PacketFactory* packetFactory;

    double initialPheromoneValue;
    unsigned int routeDiscoveryTimeoutInMilliSeconds;
    int maxNrOfRouteDiscoveryRetries;

private:
    Logger* logger = nullptr;
    unsigned int nextSequenceNumber = 1;
    std::unordered_map<AddressPtr, std::unordered_set<unsigned int>*, AddressHash, AddressPredicate> lastReceivedPackets;
};

} /* namespace ARA */
#endif /* ABSTRACTARACLIENT_H_ */
