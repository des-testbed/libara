/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "Environment.h"

#include <cassert>

using namespace std;

namespace ARA {

AbstractEARAClient::AbstractEARAClient(EARAConfiguration& configuration, EnergyAwareRoutingTable* routingTable) {
    initializeEARA(configuration, routingTable);
}

void AbstractEARAClient::initializeEARA(EARAConfiguration& configuration, EnergyAwareRoutingTable* routingTable) {
    AbstractARAClient::initialize(configuration, routingTable);
    energyDisseminationTimer = Environment::getClock()->getNewTimer();
    energyDisseminationTimer->addTimeoutListener(this);
    energyDisseminationTimer->run(configuration.getEnergyDisseminationTimeout());
    this->routingTable = routingTable;
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

void AbstractEARAClient::handlePacket(Packet* packet, NetworkInterface* interface) {
    if(packet->getType() == PacketType::ENERGY_INFO) {
        handleEnergyInfoPacket(packet);
    }
    else {
        AbstractARAClient::handlePacket(packet, interface);
    }
}

void AbstractEARAClient::handleEnergyInfoPacket(Packet* packet) {
    assert(packet->getPayloadLength() == 1);
    AddressPtr nodeAddress = packet->getSource();
    const char* payload = packet->getPayload();
    unsigned char newEnergyLevel = payload[0];
    routingTable->updateEnergyOfNode(nodeAddress, newEnergyLevel);
    delete packet;
}

} /* namespace ARA */
