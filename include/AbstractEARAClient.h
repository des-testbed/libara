/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_EARA_CLIENT_H_
#define ABSTRACT_EARA_CLIENT_H_

#include "AbstractARAClient.h"
#include "EARAConfiguration.h"
#include "EARAPacketFactory.h"
#include "EARAPacket.h"
#include "EnergyAwareRoutingTable.h"

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

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

protected:

    /**
     * Starts the calculation of the initial pheromone and energy values and creates a new route in the routing table.
     */
    virtual void createNewRouteFrom(Packet* packet, NetworkInterface* interface);

    float calculateInitialEnergyValue(EARAPacket* packet);

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

    unsigned int maximumEnergyValue;

    // `b` parameter for energy fitness initialization
    float influenceOfMinimumEnergyValue;
};

} /* namespace ARA */
#endif /* ABSTRACT_EARA_CLIENT_H_ */
