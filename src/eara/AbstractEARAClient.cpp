/*
 * $FU-Copyright$
 */

#include "AbstractEARAClient.h"
#include "Environment.h"

#include <cassert>

ARA_NAMESPACE_BEGIN

AbstractEARAClient::AbstractEARAClient(EARAConfiguration& configuration) {
    initializeEARA(configuration);
}

void AbstractEARAClient::initializeEARA(EARAConfiguration& configuration) {
    AbstractARAClient::initialize(configuration);
    routingTable = configuration.getEnergyAwareRoutingTable();
    packetFactory = configuration.getEARAPacketFactory();
    this->maximumEnergyValue = configuration.getMaximumEnergyValue();
    this->influenceOfMinimumEnergyValue = configuration.getInfluenceOfMinimumEnergyValue();
}

void AbstractEARAClient::createNewRouteFrom(Packet* packet, NetworkInterface* interface) {
    float initialPheromoneValue = calculateInitialPheromoneValue(packet->getTTL());
    float initialEnergyValue = calculateInitialEnergyValue((EARAPacket*)packet);
    routingTable->update(packet->getSource(), packet->getSender(), interface, initialPheromoneValue, initialEnergyValue);
    logTrace("Created new route to %s via %s (phi=%.2f, energy=%.2f)", packet->getSourceString().c_str(), packet->getSenderString().c_str(), initialPheromoneValue, initialEnergyValue);
}

float AbstractEARAClient::calculateInitialEnergyValue(EARAPacket* packet) {
    int nrOfHops = packetFactory->getMaximumNrOfHops() - packet->getTTL();
    assert(nrOfHops > 0);

    if (nrOfHops == 1) {
        // packet has been directly received from the source
        return packet->getTotalEnergyValue();
    }
    else {
        // decrease number of hops, because the energy of the last hop (current node has not been added to the packet)
        nrOfHops -= 1;
        float averageValue = packet->getTotalEnergyValue() / (float) nrOfHops;
        float averagePercent = averageValue / (float) maximumEnergyValue;
        float minimumPercent = packet->getMinimumEnergyValue() / (float) maximumEnergyValue;

        return averagePercent - ( (averagePercent - minimumPercent) / influenceOfMinimumEnergyValue );
    }
}

void AbstractEARAClient::broadCast(Packet* packet) {
    EARAPacket* earaPacket = (EARAPacket*) packet;
    unsigned int energyOfCurrentNode = getCurrentEnergyLevel();
    earaPacket->addEnergyValue(energyOfCurrentNode);
    AbstractARAClient::broadCast(earaPacket);
}

ARA_NAMESPACE_END
