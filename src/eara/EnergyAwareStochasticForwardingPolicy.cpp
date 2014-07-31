/*
 * $FU-Copyright$
 */

#include "EnergyAwareStochasticForwardingPolicy.h"
#include "EnergyAwareRoutingTable.h"
#include "RoutingTableEntry.h"
#include "Exception.h"

#include <cmath>
#include <deque>
#include <cassert>

ARA_NAMESPACE_BEGIN

EnergyAwareStochasticForwardingPolicy::EnergyAwareStochasticForwardingPolicy(EnergyAwareRoutingTable* routingTable, float pheromoneWeight, float energyWeight) : StochasticForwardingPolicy(routingTable) {
    this->routingTable = routingTable;
    this->pheromoneWeight = pheromoneWeight;
    this->energyWeight = energyWeight;
}

NextHop* EnergyAwareStochasticForwardingPolicy::getNextHop(const Packet* packet) {
    std::deque<RoutingTableEntry*> possibleNextHops = routingTable->getPossibleNextHops(packet);
    unsigned int nrOfPossibleNextHops = possibleNextHops.size();

    if (nrOfPossibleNextHops > 0) {

        float sum = 0.0;
        float products[nrOfPossibleNextHops] = {};

        for (unsigned int i = 0; i < nrOfPossibleNextHops; i++) {
            EARARoutingTableEntry* entry = dynamic_cast<EARARoutingTableEntry*>(possibleNextHops.at(i));
            assert(entry);

            float pheromoneValue = entry->getPheromoneValue();
            float energyValue = entry->getEnergyValue();

            float potentiatedPheromoneValue = pow(pheromoneValue, pheromoneWeight);
            float potentiatedEnergyValue = pow(energyValue, energyWeight);

            products[i] = potentiatedPheromoneValue * potentiatedEnergyValue;
            sum += products[i];
        }

        float probabilities[nrOfPossibleNextHops] = {};

        for (unsigned int i = 0; i < nrOfPossibleNextHops; i++) {
            probabilities[i] = products[i] / sum;
        }

        float cumulativeSum[nrOfPossibleNextHops];
        std::partial_sum(probabilities, probabilities + nrOfPossibleNextHops, cumulativeSum);

        int nodeIndex = getRandomNodeIndex(cumulativeSum);
        return possibleNextHops.at(nodeIndex)->getNextHop();
    }
    // DEBUG:
    std::cerr << "[EnergyAwareStochasticForwardingPolicy::getNextHop] no next hop found" << std::endl;

    return nullptr;
}

int EnergyAwareStochasticForwardingPolicy::getRandomNodeIndex(float cumulativeSum[]) {
    float randomNumber = this->getRandomNumber();
    int nodeIndex = 0;
    while (randomNumber > cumulativeSum[nodeIndex]) {
        nodeIndex += 1;
    }

    return nodeIndex;
}

float EnergyAwareStochasticForwardingPolicy::getPheromoneWeight() {
    return pheromoneWeight;
}

float EnergyAwareStochasticForwardingPolicy::getEnergyWeight() {
    return energyWeight;
}

void EnergyAwareStochasticForwardingPolicy::setPheromoneWeight(float pheromoneWeight){
    this->pheromoneWeight = pheromoneWeight;
}

void EnergyAwareStochasticForwardingPolicy::setEnergyWeight(float energyWeight){
    this->energyWeight = energyWeight;
}

ARA_NAMESPACE_END
