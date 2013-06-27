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
    energyDisseminationTimeoutInMillis = configuration.getEnergyDisseminationTimeout();
    energyDisseminationTimer = Environment::getClock()->getNewTimer();
    energyDisseminationTimer->addTimeoutListener(this);
    energyDisseminationTimer->run(energyDisseminationTimeoutInMillis * 1000);
    this->routingTable = routingTable;
}

AbstractEARAClient::~AbstractEARAClient() {
    DELETE_IF_NOT_NULL(energyDisseminationTimer);
}

void AbstractEARAClient::timerHasExpired(Timer* responsibleTimer) {
    if(responsibleTimer == energyDisseminationTimer) {
        //TODO this will be removed soon
        //sendEnergyDisseminationPacket();
        responsibleTimer->run(energyDisseminationTimeoutInMillis * 1000);
    }
    else {
        AbstractARAClient::timerHasExpired(responsibleTimer);
    }
}

ARA_NAMESPACE_END
