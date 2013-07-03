/*
 * $FU-Copyright$
 */

#include "StochasticForwardingPolicy.h"
#include "Exception.h"

ARA_NAMESPACE_BEGIN

NextHop* StochasticForwardingPolicy::getNextHop(const Packet* packet) {
    std::deque<RoutingTableEntry*> possibleNextHops = routingTable->getPossibleNextHops(packet);
    if(possibleNextHops.empty()) {
        throw Exception("Could not determine next hop: there are no known routes to the destination");
    }

    // check if the sender is among the possible next hops and remove it
    for (std::deque<RoutingTableEntry*>::iterator entry = possibleNextHops.begin(); entry != possibleNextHops.end(); entry++) {
        if ((*entry)->getAddress()->equals(packet->getSender())) {
            possibleNextHops.erase(entry);
            break;
        }
    }

    unsigned int nrOfPossibleNextHops = possibleNextHops.size();
    float cumulativeSum[nrOfPossibleNextHops];
    float probabilities[nrOfPossibleNextHops];
    float pheromoneValues[nrOfPossibleNextHops];

    float sumOfPheromoneValues = 0;
    for(unsigned int i = 0; i < nrOfPossibleNextHops; i++){
        pheromoneValues[i] = possibleNextHops.at(i)->getPheromoneValue();
        sumOfPheromoneValues += pheromoneValues[i];
    }

    // calculate the probabilities
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

    NextHop* result = possibleNextHops.at(nodeIndex)->getNextHop();
    return result;
}

void StochasticForwardingPolicy::initializeRandomNumberGenerator(unsigned int seed) {
    srand(seed);
}

float StochasticForwardingPolicy::getRandomNumber() {
    // not sure if this really works
    return (float)rand() / (float)RAND_MAX;
}

ARA_NAMESPACE_END
