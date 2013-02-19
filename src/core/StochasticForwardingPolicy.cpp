/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große, Michael Frey
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#include "StochasticForwardingPolicy.h"

using namespace ARA;

// todo: add exception for "no hop available", are not yet interfaces are not yet considered
NextHop* StochasticForwardingPolicy::getNextHop(const Packet* packet) {
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

    // pick a random number, TODO: check how to generate random numbers in omnet++
    // q = genk_dblrand(0);

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

void StochasticForwardingPolicy::setRoutingTable(RoutingTable* routingTable){
    this->routingTable = routingTable;
}

