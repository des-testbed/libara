/*
 * $FU-Copyright$
 */

#ifndef ABSTRACTARACLIENT_H_
#define ABSTRACTARACLIENT_H_

#include "ARAMacros.h"
#include "AbstractNetworkClient.h"
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
#include "Time.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <memory>
#include <string>

ARA_NAMESPACE_BEGIN

typedef std::unordered_map<AddressPtr, TimerPtr, AddressHash, AddressPredicate> RunningRouteDiscoveriesMap;
typedef std::unordered_map<AddressPtr, std::unordered_set<unsigned int>*, AddressHash, AddressPredicate> LastReceivedPacketsMap;
typedef std::unordered_map<AddressPtr, std::unordered_set<AddressPtr>*, AddressHash, AddressPredicate> KnownIntermediateHopsMap;
typedef std::unordered_map<AddressPtr, unsigned int, AddressHash, AddressPredicate> LastRouteDiscoveriesMap;
typedef std::unordered_map<AddressPtr, std::pair<Time*, NetworkInterface*>, AddressHash, AddressPredicate> NeighborActivityMap;
typedef std::unordered_map<AddressPtr, TimerPtr, AddressHash, AddressPredicate> ScheduledPANTsMap;
typedef std::unordered_set<TimerPtr> DeliveryTimerSet;

/**
 * TODO write class description
 */
class AbstractARAClient : public virtual AbstractNetworkClient, public TimeoutEventListener {

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
    AbstractARAClient(Configuration& configuration);

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
    virtual void sendPacket(Packet* packet);

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
     * @return False if the packet could not be handled and has been dropped, True otherwise
     */
    virtual bool handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface);

    /**
     * This method will initialize this client with the given configuration, ~RoutingTable and ~PacketFactory.
     * It must be called before any call to AbstractARAClient::sendPacket or
     * AbstractARAClient::receivePacket. If this object has been created by the
     * standard constructor this method must be called manually.
     */
    void initialize(Configuration& configuration);

    virtual void timerHasExpired(TimerPtr responsibleTimer);

    void setMaxNrOfRouteDiscoveryRetries(int maxNrOfRouteDiscoveryRetries);

    void setForwardingPolicy(ForwardingPolicy* newForwardingPolicy);

    int getMaxTTL() const;

protected:

    virtual void sendUnicast(Packet* packet, NetworkInterface* interface, AddressPtr receiver);

    /**
     * This method either initializes or reinforces a route in the routing table.
     */
    virtual void updateRoutingTable(Packet* packet, NetworkInterface* interface);

    /**
     * Calculates an initial pheromone value based on the initialPhi value from the Configuration
     * and the given TTL with the formula with alpha=1.
     * result = alpha * TTL + initialPheromoneValue
     */
    float calculateInitialPheromoneValue(unsigned int ttl);

    /**
     * Handles path reinforcement using the currently set PathReinforcementPolicy.
     * The new pheromone value is returned.
     */
    virtual float reinforcePheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

    /**
     * Checks if the given packet has been received before by checking the source address and
     * sequence number field.
     */
    virtual bool hasBeenReceivedEarlier(const Packet* packet);

    /**
     * This will register the source address and sequence number of the given packet
     * so we can determine if we have seen a packet before and also detect loops.
     * If the previous hop feature is enabled, this will also remember the previous hop
     * of this packet.
     */
    void registerReceivedPacket(const Packet* packet);

    /**
     * Handles a packet depending on its type. This method is protected virtual to enable
     * other ARA implementations (like EARA) to override it to add new packet types.
     */
    virtual void handlePacket(Packet* packet, NetworkInterface* interface);

    void handleDuplicatePacket(Packet* packet, NetworkInterface* interface);
    void sendDuplicateWarning(Packet* packet, NetworkInterface* interface);
    void handleDataPacket(Packet* packet);
    virtual void handleDataPacketForThisNode(Packet* packet);
    virtual void handleAntPacket(Packet* packet, NetworkInterface* interface);
    void handleAntPacketForThisNode(Packet* packet);
    void broadcastBANT(Packet* fant);
    void handleBANTForThisNode(Packet* bant);
    virtual void handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface);
    void handleRouteFailurePacket(Packet* packet, NetworkInterface* interface);
    virtual void startNewRouteDiscovery(Packet* packet);
    void startRouteDiscoveryTimer(const Packet* packet);
    void forgetKnownIntermediateHopsFor(AddressPtr destination);
    void broadcastFANT(AddressPtr destination);
    bool isRouteDiscoveryRunning(AddressPtr destination);
    virtual void handleNonSourceRouteDiscovery(Packet* packet);
    virtual void handlePacketWithZeroTTL(Packet* packet);
    void stopRouteDiscoveryTimer(AddressPtr destination);
    void startDeliveryTimer(AddressPtr destination);
    void sendDeliverablePackets(AddressPtr destination);
    virtual void createNewRouteFrom(Packet* packet, NetworkInterface* interface);
    bool hasPreviousNodeBeenSeenBefore(const Packet* packet);
    void deleteRoutingTableEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
    void broadcastRouteFailure(AddressPtr destination);
    void broadcastPANT(AddressPtr destination);
    void checkPantTimer(const Packet* packet);

    void handleExpiredRouteDiscoveryTimer(TimerPtr routeDiscoveryTimer);
    void handleExpiredDeliveryTimer(TimerPtr deliveryTimer);
    void handleExpiredPANTTimer(TimerPtr pantTimer);

    void startNeighborActivityTimer();
    void registerActivity(AddressPtr neighbor, NetworkInterface* interface);
    void checkInactiveNeighbors();
    bool isNewRouteDiscovery(const Packet* packet);

    /**
     * A small convenience method to retrieve a timer from the static Environment.
     */
    TimerPtr getNewTimer(char timerType, void* contextObject=nullptr) const;

protected:
    TimerPtr neighborActivityTimer;

    RunningRouteDiscoveriesMap runningRouteDiscoveries;
    ScheduledPANTsMap scheduledPANTs;
    DeliveryTimerSet runningDeliveryTimers;

    ForwardingPolicy* forwardingPolicy;
    PathReinforcementPolicy* pathReinforcementPolicy;
    EvaporationPolicy* evaporationPolicy;

    double initialPheromoneValue;
    unsigned int packetDeliveryDelayInMilliSeconds;
    unsigned int routeDiscoveryTimeoutInMilliSeconds;
    unsigned int neighborActivityCheckIntervalInMilliSeconds;
    long maxNeighborInactivityTimeInMilliSeconds;
    unsigned int pantIntervalInMilliSeconds;
    bool isPreviousHopFeatureActivated;
    int maxNrOfRouteDiscoveryRetries;

    //TODO the knownIntermediateHops and lastReceivedPackets may be merged into a single hashmap
    LastReceivedPacketsMap lastReceivedPackets;

    /**
     * This hashmap records the seen hops to a specific destination. This includes direct neighbors as well as indirect
     * nodes this client has learned from the penultimate packet field.
     */
    KnownIntermediateHopsMap knownIntermediateHops;

    /**
     * This hashmap holds information of the last time a neighbor has shown some activity.
     * This could either be that this client has successfully received or send a packet to/from this client
     */
    NeighborActivityMap neighborActivityTimes;

    /**
     * We identify each route discovery by its origin (source address) and sequence number.
     * When we receive a new ant packet we check its sequence number with this. If they differ,
     * this is a new route discovery and we need to forget about all knownIntermediateHops for
     * the packets source we know so far.
     */
    LastRouteDiscoveriesMap lastRouteDiscoverySeqNumbers;
};

ARA_NAMESPACE_END

#endif /* ABSTRACTARACLIENT_H_ */
