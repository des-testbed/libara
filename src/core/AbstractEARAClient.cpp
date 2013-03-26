/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "Environment.h"

using namespace std;

namespace ARA {

AbstractEARAClient::AbstractEARAClient(Configuration& configuration, EnergyAwareRoutingTable* routingTable) {
    initialize(configuration, routingTable);
}

void AbstractEARAClient::initialize(EARAConfiguration& configuration, RoutingTable *routingTable) {
    AbstractARAClient::initialize(configuration, routingTable);
    energyDisseminationTimer = Environment::getClock()->getNewTimer();
    energyDisseminationTimer->run(configuration.getEnergyDisseminationTimeout());
}

AbstractEARAClient::~AbstractEARAClient() {
    delete energyDisseminationTimer;
}

void AbstractEARAClient::timerHasExpired(Timer* responsibleTimer) {
    if(responsibleTimer == energyDisseminationTimer) {
        sendEnergyDisseminationPacket();
    }
    else {
        AbstractARAClient::timerHasExpired(responsibleTimer);
    }
}

void AbstractEARAClient::sendEnergyDisseminationPacket() {
    for(auto& interface: interfaces) {
        AddressPtr interfaceAddress = interface->getLocalAddress();
        unsigned int seqNr = getNextSequenceNumber();
        unsigned char currentEnergyLevel = getCurrentEnergyLevel();
        Packet* energyPacket = packetFactory->makeEnergyDisseminationPacket(interfaceAddress, seqNr, currentEnergyLevel);
        interface->broadcast(energyPacket);
    }
}

} /* namespace ARA */
