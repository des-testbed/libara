/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_EARA_CLIENT_H_
#define ABSTRACT_EARA_CLIENT_H_

#include "ARAMacros.h"
#include "AbstractARAClient.h"
#include "EARAConfiguration.h"
#include "EARAPacketFactory.h"
#include "EARAPacket.h"
#include "EnergyAwareRoutingTable.h"
#include "EARAForwardingPolicy.h"

ARA_NAMESPACE_BEGIN

typedef std::unordered_map<AddressPtr, TimerPtr, AddressHash, AddressPredicate> RouteDiscoveryDelayTimerMap;

struct AntPacketRouteFitness {
    Packet* packet;
    float routeEnergyFitness;
};

/**
 * TODO write class description
 */
class AbstractEARAClient : public AbstractARAClient {

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
    AbstractEARAClient() {}

    /**
     * This is the recommended constructor which should be used by all concrete implementation
     * if possible. It will initialize the client with the given configuration so no additional
     * call to AbstractARAClient::initialize is required.
     */
    AbstractEARAClient(EARAConfiguration& configuration);

    virtual ~AbstractEARAClient();

    /**
     * Initializes the EARE specific part of this class.
     */
    void initializeEARA(EARAConfiguration& configuration);

    /**
     * This method must be implemented by the concrete EARA client. It returns the current energy
     * level. An energy value of 0 indicates that the energy is as good as depleted.
     * The upper limit should be the maximum energy capacity.
     */
    virtual unsigned int getCurrentEnergyLevel() = 0;

    /**
     * This method is overwritten to embed the energy values in the packets before they are
     * actually broadcasted on the network interface.
     */
    virtual void broadCast(Packet* packet);

    virtual void timerHasExpired(std::weak_ptr<Timer> responsibleTimer);

    /**
     * Returns the percentage (between 1 and 100) that an energy value in the routing table represents
     * This will be only used in tests and for output but not in the calculations itself.
     */
    float getEnergyPercentage(float energyValue) const;

protected:

    virtual void sendUnicast(Packet* packet, NetworkInterface* interface, AddressPtr receiver);

    virtual void updateRoutingTable(Packet* packet, NetworkInterface* interface);

    virtual void createNewRouteFrom(Packet* packet, NetworkInterface* interface);

    float calculateInitialEnergyValue(EARAPacket* packet);

    virtual bool hasBeenReceivedEarlier(const Packet* packet);

    virtual void handleAntPacket(Packet* packet, NetworkInterface* interface);

    /**
     * Checks if a route discovery delay timer is already running for the source of the given packet.
     * If none is running, a new route discovery delay timer is started.
     * If there is already a timer running, the given FANT or BANT is compared to the currently best
     * FANT/BANT in terms of their TTL and energy metric.
     * Only the best ant packet is stored in the timer context Object and will be broadcasted
     * when the timer expires.
     */
    void handleAntPacketWithDelayTimer(Packet* packet, NetworkInterface* interface);

    /**
     * Starts a new route discovery delay timer and stores the given packet and routeEnergy as context object for that timer.
     */
    void startNewRouteDiscoveryDelayTimer(Packet* antPacket, float routeEnergyOfNewAnt);

    /**
     * This calculates the fitness of the route a packet has traveled on.
     * It is used in the route discovery process together with the route discovery delay
     * to determine which FANT/BANT should be broadcasted further into the network.
     */
    float calculateRouteFitness(int ttl, float energyFitness);

    float normalizeEnergyValue(float energyValue) const;

    void handleExpiredRouteDiscoveryDelayTimer(std::weak_ptr<Timer> timer);

    virtual void handleDataPacketForThisNode(Packet* packet);

    void broadcastPEANT();

    void addEnergyInformationToPacket(Packet* packet);

private:
    /**
     * This method is private to prevent anyone from using it because we slightly changed
     * the method signature to require an instance of EARAConfiguration.
     * @see AbstractEARAClient::initializeEARA
     */
    void initialize(Configuration& configuration, RoutingTable *routingTable, PacketFactory* packetFactory) {};

protected:
    EnergyAwareRoutingTable* routingTable;
    EARAPacketFactory* packetFactory;
    EARAForwardingPolicy* forwardingPolicy;
    RouteDiscoveryDelayTimerMap runningRouteDiscoveryDelayTimers;

    /**
     * Maximum energy capacity of this node
     */
    unsigned int maximumEnergyValue;

    /**
     * Maximum energy capacity in the whole network.
     * This is used for normalization.
     */
    unsigned int maximumBatteryCapacityInNetwork;

    unsigned int routeDiscoveryDelayInMilliSeconds;

    // `b` parameter for energy fitness initialization
    float influenceOfMinimumEnergyValue;

    // used to determine when PEANTs shall be send
    float peantEnergyThreshold;
    unsigned int energyLevelWhenLastPEANTHasBeenSent;
};

ARA_NAMESPACE_END

#endif /* ABSTRACT_EARA_CLIENT_H_ */
