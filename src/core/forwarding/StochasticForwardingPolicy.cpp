/*
 * $FU-Copyright$
 */

#include "StochasticForwardingPolicy.h"

ARA_NAMESPACE_BEGIN

StochasticForwardingPolicy::StochasticForwardingPolicy(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

NextHop* StochasticForwardingPolicy::getNextHop(const Packet* packet) {
    std::deque<RoutingTableEntry*> possibleNextHops = routingTable->getPossibleNextHops(packet);

    if (possibleNextHops.empty()) {
        throw Exception("Could not determine next hop: there are no known routes to the destination");
    }

    unsigned int nrOfPossibleNextHops = possibleNextHops.size();

    if (nrOfPossibleNextHops > 0) {
        std::vector<float> cumulativeSum(nrOfPossibleNextHops);
        std::vector<float> probabilities(nrOfPossibleNextHops);
        std::vector<float> pheromoneValues(nrOfPossibleNextHops);

        float sumOfPheromoneValues = 0;

        for (unsigned int i = 0; i < nrOfPossibleNextHops; i++) {
            pheromoneValues[i] = possibleNextHops.at(i)->getPheromoneValue();
            sumOfPheromoneValues += pheromoneValues[i];
        }

        // calculate the probabilities
        for (unsigned int i = 0; i < nrOfPossibleNextHops; i++) {
            probabilities[i] = pheromoneValues[i] / sumOfPheromoneValues;
        }

        // compute the cumulative sum
        std::partial_sum(probabilities.begin(), probabilities.end(), cumulativeSum.begin());

        // get a random number between 0.0 and 1.0
        float randomNumber = this->getRandomNumber();

        int nodeIndex = 0;

        while (randomNumber > cumulativeSum[nodeIndex]) {
            nodeIndex += 1;
        }

        NextHop* result = possibleNextHops.at(nodeIndex)->getNextHop();
        return result;
    }
    // DEBUG:
    std::cerr << "[StochasticForwardingPolicy::getNextHop] no next hop available" << std::endl;

    return nullptr;
}

void StochasticForwardingPolicy::initializeRandomNumberGenerator(unsigned int seed) {
    srand(seed);
}

float StochasticForwardingPolicy::getRandomNumber() {
    return (float)random() / (float)RAND_MAX;
}

ARA_NAMESPACE_END
