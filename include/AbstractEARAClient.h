/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_EARA_CLIENT_H_
#define ABSTRACT_EARA_CLIENT_H_

#include "AbstractARAClient.h"
#include "EARAConfiguration.h"
#include "PacketFactory.h"
#include "EnergyAwareRoutingTable.h"
#include "Timer.h"

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
    AbstractEARAClient(EARAConfiguration& configuration, EnergyAwareRoutingTable *routingTable, PacketFactory* packetFactory);

    /**
     * The standard virtual destructor of this abstract class.
     */
    virtual ~AbstractEARAClient();

    void initializeEARA(EARAConfiguration& configuration, EnergyAwareRoutingTable* routingTable, PacketFactory* packetFactory);

    /**
     * This method must be implemented by the concrete EARA client. It returns the current energy
     * level in a range between 0 and 255. An energy level of 255 means full battery capactity
     * and a level of 0 indicates that the energy is as good as depleted.
     */
    virtual unsigned char getCurrentEnergyLevel() = 0;

    void timerHasExpired(Timer* responsibleTimer);

protected:
    /**
     * Method is overriden to implement the handling of the energy information packets.
     * @see AbstractARAClient::handlePacket
     */
    void handlePacket(Packet* packet, NetworkInterface* interface);

private:
    /**
     * This method is private to prevent anyone from using it because we slightly changed
     * the method signature to require an instance of EARAConfiguration.
     * @see AbstractEARAClient::initializeEARA
     */
    void initialize(Configuration& configuration, RoutingTable *routingTable, PacketFactory* packetFactory) {};

    void sendEnergyDisseminationPacket();
    void handleEnergyInfoPacket(Packet* packet);

protected:
    Timer* energyDisseminationTimer;
    EnergyAwareRoutingTable* routingTable;
};

} /* namespace ARA */
#endif /* ABSTRACT_EARA_CLIENT_H_ */
