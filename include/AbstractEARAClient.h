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

ARA_NAMESPACE_BEGIN

typedef std::unordered_map<AddressPtr, Timer*, AddressHash, AddressPredicate> RouteDiscoveryDelayTimerMap;

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
     * level in a range between 0 and 255. An energy level of 255 means full battery capacity
     * and a level of 0 indicates that the energy is as good as depleted.
     */
    virtual unsigned char getCurrentEnergyLevel() = 0;

    /**
     * This method is overwritten to embed the energy values in the packets before they are
     * actually broadcasted on the network interface.
     */
    virtual void broadCast(Packet* packet);

    virtual void timerHasExpired(Timer* responsibleTimer);

protected:

    /**
     * Starts the calculation of the initial pheromone and energy values and creates a new route in the routing table.
     */
    virtual void createNewRouteFrom(Packet* packet, NetworkInterface* interface);

    float calculateInitialEnergyValue(EARAPacket* packet);

    virtual void handleAntPacket(Packet* packet);

    /**
     * Checks if a route discovery delay timer is already running for the source of the given packet.
     * If none is running, a new route discovery delay timer is started.
     * If there is already a timer running, the given FANT or BANT is compared to the currently best
     * FANT/BANT in terms of their TTL and energy metric.
     * Only the best ant packet is stored in the timer context Object and will be broadcasted
     * when the timer expires.
     */
    void handleFANTorBANT(Packet* packet);

    void handleExpiredRouteDiscoveryDelayTimer(Timer* timer);

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
    RouteDiscoveryDelayTimerMap runningRouteDiscoveryDelayTimers;

    unsigned int maximumEnergyValue;
    unsigned int routeDiscoveryDelayInMilliSeconds;

    // `b` parameter for energy fitness initialization
    float influenceOfMinimumEnergyValue;
};

ARA_NAMESPACE_END

#endif /* ABSTRACT_EARA_CLIENT_H_ */
