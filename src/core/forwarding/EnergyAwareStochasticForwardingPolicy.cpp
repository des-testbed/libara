#include "EnergyAwareStochasticForwardingPolicy.h"

using namespace ARA;

NextHop* EnergyAwareStochasticForwardingPolicy::getNextHop(const Packet *packet){
    std::deque<RoutingTableEntry*>* possibleNextHops = routingTable->getPossibleNextHops(packet);
    unsigned int nrOfPossibleNextHops = possibleNextHops->size();

    float sum = .0;

    float cumulativeSum[nrOfPossibleNextHops];
    float probabilities[nrOfPossibleNextHops];

    float pheromoneValues[nrOfPossibleNextHops];
    float energyValues[nrOfPossibleNextHops];

    for(unsigned int i = 0; i < nrOfPossibleNextHops; i++){
        pheromoneValues[i] = possibleNextHops->at(i)->getPheromoneValue();
		// FIXME: we need somehow to get the energy values of possible next hops
		//energyValues[i] = possibleNextHops->at(i)->getEnergyValue();
		energyValues[i] = 1;
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
