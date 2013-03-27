#include "EnergyAwareStochasticForwardingPolicy.h"

using namespace ARA;

EnergyAwareStochasticForwardingPolicy::EnergyAwareStochasticForwardingPolicy(float pAlpha, float pBeta):alpha(pAlpha),beta(pBeta){ }

NextHop* EnergyAwareStochasticForwardingPolicy::getNextHop(const Packet *packet, RoutingTable *routingTable){
    std::deque<RoutingTableEntry*>* possibleNextHops = routingTable->getPossibleNextHops(packet);
    unsigned int nrOfPossibleNextHops = possibleNextHops->size();

    float sum = .0;

    float cumulativeSum[nrOfPossibleNextHops];
    float probabilities[nrOfPossibleNextHops];

    float pheromoneValues[nrOfPossibleNextHops];
    float energyValues[nrOfPossibleNextHops];

    for(unsigned int i = 0; i < nrOfPossibleNextHops; i++){
        try {
            EnergyAwareRoutingTableEntry *entry = static_cast<EnergyAwareRoutingTableEntry*>(possibleNextHops->at(i));
            pheromoneValues[i] = entry->getPheromoneValue();
	   	    energyValues[i] = entry->getEnergyValue();
            sum += (pow(pheromoneValues[i], alpha) * pow(energyValues[i], beta));
        } catch (std::bad_cast &exp) {
            throw("EnergyAwareStochasticForwardingPolicy: cast to EnergyAwareRoutingTableEntry failed");
        }
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

    NextHop* result = possibleNextHops->at(nodeIndex)->getNextHop();
    //FIXME possibleNextHops muss noch gelöscht werden!
    return result;
}

void EnergyAwareStochasticForwardingPolicy::setPheromoneWeight(float alpha){
  this->alpha = alpha;
}

void EnergyAwareStochasticForwardingPolicy::setEnergyWeight(float beta){
  this->beta = beta;
}
