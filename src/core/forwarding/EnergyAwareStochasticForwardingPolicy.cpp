/*
 * $FU-Copyright$
 */

#include "EnergyAwareStochasticForwardingPolicy.h"
#include "EnergyAwareRoutingTable.h"
#include "RoutingTableEntry.h"
#include "Exception.h"

using namespace ARA;

EnergyAwareStochasticForwardingPolicy::EnergyAwareStochasticForwardingPolicy(float pAlpha, float pBeta):alpha(pAlpha),beta(pBeta){ }

NextHop* EnergyAwareStochasticForwardingPolicy::getNextHop(const Packet *packet, RoutingTable *routingTable){
    try {
        EnergyAwareRoutingTable* energyAwareRoutingTable = static_cast<EnergyAwareRoutingTable*>(routingTable);

        std::deque<RoutingTableEntry*> possibleNextHops = routingTable->getPossibleNextHops(packet);
        unsigned int nrOfPossibleNextHops = possibleNextHops.size();

        float sum = 0.0;

        float cumulativeSum[nrOfPossibleNextHops];
        float probabilities[nrOfPossibleNextHops];

        float pheromoneValues[nrOfPossibleNextHops];
        float energyValues[nrOfPossibleNextHops];

        for(unsigned int i = 0; i < nrOfPossibleNextHops; i++){
            RoutingTableEntry* entry = possibleNextHops.at(i);
            pheromoneValues[i] = entry->getPheromoneValue();
            energyValues[i] = energyAwareRoutingTable->getEnergyValueOf(entry->getAddress()); //FIXME what if we do not have any energy information for this node
            sum += (pow(pheromoneValues[i], alpha) * pow(energyValues[i], beta));
        }

        for(unsigned int i = 0; i < nrOfPossibleNextHops; i++){
            probabilities[i] = (pow(pheromoneValues[i], alpha) * pow(energyValues[i], beta)) / sum;
        }

        std::partial_sum(probabilities, probabilities + nrOfPossibleNextHops, cumulativeSum);

        float randomNumber = this->getRandomNumber();

        int nodeIndex = 0;
        while(randomNumber > cumulativeSum[nodeIndex]){
            nodeIndex += 1;
        }

        NextHop* result = possibleNextHops.at(nodeIndex)->getNextHop();
        //FIXME possibleNextHops muss noch gelöscht werden!
        return result;
    } catch (std::bad_cast& exp) {
        throw Exception("EnergyAwareStochasticForwardingPolicy: cast to EnergyAwareRoutingTable failed");
    }
}

void EnergyAwareStochasticForwardingPolicy::setPheromoneWeight(float alpha){
    this->alpha = alpha;
}

void EnergyAwareStochasticForwardingPolicy::setEnergyWeight(float beta){
    this->beta = beta;
}
