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

StochasticForwardingPolicy::StochasticForwardingPolicy(RoutingTable& pRoutingTable):AbstractForwardingPolicy(pRoutingTable){}

StochasticForwardingPolicy::~StochasticForwardingPolicy(){}

// todo: add exception for "no hop available", are not yet interfaces are not yet considered
NextHop StochasticForwardingPolicy::getNextHop(Packet& pPacket){
  int node = 0;
  /// get a list of possible nodes towards a destination
  LinkedList<RoutingTableEntry> *list = this->mRoutingTable.getPossibleNextHops(&pPacket);
  /// store the size of the possible node list
  unsigned int size = list->size();
  /// the sum in the probablity function
  float sum = .0;
  /// the cumulated sum of the probablities 
  float probabilities[size];
  /// the cumulated sum of the probablities 
  float c_sum[size];
  /// the pheromone values of the 
  float pheromones[size];
  // iterate over the pherome values
  for(unsigned int i = 0; i < size; i++){
    pheromones[i] = list->get(i)->getPheromoneValue();
    sum += pheromones[i];
  }

  // determine the probabilities
  for(unsigned int i = 0; i < size; i++){
	probabilities[i] = pheromones[i]/sum;
  }

  // compute the cumulated sum
  std::partial_sum(probabilities, probabilities + size, c_sum);

  // pick a random number, TODO: check how to generate random numbers in omnet++
  //q = genk_dblrand(0);
  //
  
  //while q > c_sum[node]:
  //  node += 1

  // fixme
  int index = 0;

  // create the result 
  NextHop result = NextHop(list->get(index)->getNextHop()->getAddress(), list->get(index)->getNextHop()->getInterface());
  // delete the list
  delete list;
  // todo: check ob mir das um die ohren fliegt, mehr als wahrscheinlich
  return result;
}


