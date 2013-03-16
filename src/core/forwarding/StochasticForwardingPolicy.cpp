/*
 * $FU-Copyright$
 */

#include "StochasticForwardingPolicy.h"

#include <deque>
#include <numeric>

using namespace ARA;

// todo: add exception for "no hop available", are not yet interfaces are not yet considered
NextHop* StochasticForwardingPolicy::getNextHop(const Packet* packet, RoutingTable* routingTable) {
    std::deque<RoutingTableEntry*>* possibleNextHops = routingTable->getPossibleNextHops(packet);
    unsigned int nrOfPossibleNextHops = possibleNextHops->size();

    float sumOfPheromoneValues = .0;
    float cumulativeSum[nrOfPossibleNextHops];
    float probabilities[nrOfPossibleNextHops];
    float pheromoneValues[nrOfPossibleNextHops];

    // create the sumOfPheromoneValues of pheromone values
    for(unsigned int i = 0; i < nrOfPossibleNextHops; i++){
        pheromoneValues[i] = possibleNextHops->at(i)->getPheromoneValue();
        sumOfPheromoneValues += pheromoneValues[i];
    }

    // determine the probabilities
    for(unsigned int i = 0; i < nrOfPossibleNextHops; i++){
        probabilities[i] = pheromoneValues[i] / sumOfPheromoneValues;
    }

    // compute the cumulative sum
    std::partial_sum(probabilities, probabilities + nrOfPossibleNextHops, cumulativeSum);


    // get a random number between 0.0 and 1.0
    float randomNumber = this->getRandomNumber();

    int nodeIndex = 0;
    while(randomNumber > cumulativeSum[nodeIndex]){
        nodeIndex += 1;
    }

    NextHop* result = possibleNextHops->at(nodeIndex)->getNextHop();
    //FIXME possibleNextHops muss noch gelöscht werden!
    return result;
}

void StochasticForwardingPolicy::initializeRandomNumberGenerator(){
    srand((unsigned)time(0));
}

float StochasticForwardingPolicy::getRandomNumber(){
    // not sure if this really works
    return ((float)rand()/(float)RAND_MAX);
}
