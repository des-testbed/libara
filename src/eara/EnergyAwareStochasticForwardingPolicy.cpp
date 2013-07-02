/*
 * $FU-Copyright$
 */

#include "EnergyAwareStochasticForwardingPolicy.h"
#include "EnergyAwareRoutingTable.h"
#include "RoutingTableEntry.h"
#include "Exception.h"

#include <cmath>
#include <deque>

using namespace ARA;

EnergyAwareStochasticForwardingPolicy::EnergyAwareStochasticForwardingPolicy(float pheromoneWeight, float energyWeight) {
    alpha = pheromoneWeight;
    beta = energyWeight;
}

NextHop* EnergyAwareStochasticForwardingPolicy::getNextHop(const Packet* packet, RoutingTable* routingTable) {
    try {
        EnergyAwareRoutingTable* energyAwareRoutingTable = static_cast<EnergyAwareRoutingTable*>(routingTable);

        std::deque<RoutingTableEntry*> possibleNextHops = routingTable->getPossibleNextHops(packet);
        unsigned int nrOfPossibleNextHops = possibleNextHops.size();

        float sum = 0.0;
        float products[nrOfPossibleNextHops];

        for (unsigned int i = 0; i < nrOfPossibleNextHops; i++) {
            RoutingTableEntry* entry = possibleNextHops.at(i);
            float pheromoneValue = entry->getPheromoneValue();
            //float energyValue = energyAwareRoutingTable->getEnergyValue(entry->getAddress()); //FIXME what if we do not have any energy information for this node
            //FIXME
            float energyValue = 1;

            float potentiatedPheromoneValue = pow(pheromoneValue, alpha);
            float potentiatedEnergyValue = pow(energyValue, beta);

            products[i] = potentiatedPheromoneValue * potentiatedEnergyValue;
            sum += products[i];
        }

        float probabilities[nrOfPossibleNextHops];
        for (unsigned int i = 0; i < nrOfPossibleNextHops; i++) {
            probabilities[i] = products[i] / sum;
        }

        float cumulativeSum[nrOfPossibleNextHops];
        std::partial_sum(probabilities, probabilities + nrOfPossibleNextHops, cumulativeSum);

        int nodeIndex = getRandomNodeIndex(cumulativeSum);
        return possibleNextHops.at(nodeIndex)->getNextHop();

    } catch (std::bad_cast& exception) {
        throw Exception("EnergyAwareStochasticForwardingPolicy: cast to EnergyAwareRoutingTable failed");
    }
}

int EnergyAwareStochasticForwardingPolicy::getRandomNodeIndex(float cumulativeSum[]) {
    float randomNumber = this->getRandomNumber();
    int nodeIndex = 0;
    while (randomNumber > cumulativeSum[nodeIndex]) {
        nodeIndex += 1;
    }

    return nodeIndex;
}

void EnergyAwareStochasticForwardingPolicy::setPheromoneWeight(float alpha){
    this->alpha = alpha;
}

void EnergyAwareStochasticForwardingPolicy::setEnergyWeight(float beta){
    this->beta = beta;
}
