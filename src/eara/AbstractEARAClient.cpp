/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "Environment.h"

#include <cassert>

ARA_NAMESPACE_BEGIN

AbstractEARAClient::AbstractEARAClient(EARAConfiguration& configuration, EnergyAwareRoutingTable* routingTable, PacketFactory* packetFactory) {
    initializeEARA(configuration, routingTable, packetFactory);
}

void AbstractEARAClient::initializeEARA(EARAConfiguration& configuration, EnergyAwareRoutingTable* routingTable, PacketFactory* packetFactory) {
    AbstractARAClient::initialize(configuration, routingTable, packetFactory);
    this->routingTable = routingTable;
}

ARA_NAMESPACE_END
