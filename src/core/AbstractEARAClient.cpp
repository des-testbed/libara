/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "Environment.h"

#include <cassert>

ARA_NAMESPACE_BEGIN

AbstractEARAClient::AbstractEARAClient(EARAConfiguration& configuration, PacketFactory* packetFactory) {
    initializeEARA(configuration, packetFactory);
}

void AbstractEARAClient::initializeEARA(EARAConfiguration& configuration, PacketFactory* packetFactory) {
    AbstractARAClient::initialize(configuration, packetFactory);
    energyDisseminationTimeoutInMillis = configuration.getEnergyDisseminationTimeout();
    energyDisseminationTimer = Environment::getClock()->getNewTimer();
    energyDisseminationTimer->addTimeoutListener(this);
    energyDisseminationTimer->run(energyDisseminationTimeoutInMillis * 1000);
    routingTable = configuration.getEnergyAwareRoutingTable();
}

AbstractEARAClient::~AbstractEARAClient() {
    DELETE_IF_NOT_NULL(energyDisseminationTimer);
}

void AbstractEARAClient::timerHasExpired(Timer* responsibleTimer) {
    if(responsibleTimer == energyDisseminationTimer) {
        sendEnergyDisseminationPacket();
        responsibleTimer->run(energyDisseminationTimeoutInMillis * 1000);
    }
    else {
        AbstractARAClient::timerHasExpired(responsibleTimer);
    }
}

void AbstractEARAClient::sendEnergyDisseminationPacket() {
    unsigned char currentEnergyLevel = getCurrentEnergyLevel();
    logDebug("Sending energy dissemination packet (energy at %u)", currentEnergyLevel);
    for(auto& interface: interfaces) {
        AddressPtr interfaceAddress = interface->getLocalAddress();
        unsigned int seqNr = getNextSequenceNumber();
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

ARA_NAMESPACE_END
