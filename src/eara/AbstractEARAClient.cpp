/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "Environment.h"

#include <cassert>

ARA_NAMESPACE_BEGIN

AbstractEARAClient::AbstractEARAClient(EARAConfiguration& configuration, EnergyAwareRoutingTable* routingTable, EARAPacketFactory* packetFactory) {
    initializeEARA(configuration, routingTable, packetFactory);
}

void AbstractEARAClient::initializeEARA(EARAConfiguration& configuration, EnergyAwareRoutingTable* routingTable, EARAPacketFactory* packetFactory) {
    AbstractARAClient::initialize(configuration, routingTable, packetFactory);
    this->routingTable = routingTable;
    this->packetFactory = packetFactory;
}

void AbstractEARAClient::broadCast(Packet* packet) {
    EARAPacket* earaPacket = (EARAPacket*) packet;
    unsigned int energyOfCurrentNode = getCurrentEnergyLevel();
    earaPacket->addEnergyValue(energyOfCurrentNode);
    AbstractARAClient::broadCast(earaPacket);
}

ARA_NAMESPACE_END
