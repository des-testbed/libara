/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
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

#include "BestPheromoneForwardingPolicy.h"

using namespace ARA;

BestPheromoneForwardingPolicy::BestPheromoneForwardingPolicy(RoutingTable* pRoutingTable):AbstractForwardingPolicy(pRoutingTable){}

BestPheromoneForwardingPolicy::~BestPheromoneForwardingPolicy(){ 
 // delete this->mRoutingTable;
}

// todo: add exception for "no hop available", are not yet interfaces are not yet considered
NextHop BestPheromoneForwardingPolicy::getNextHop(Packet* pPacket){
  /// get a list of possible nodes towards a destination
  LinkedList<RoutingTableEntry>* list = this->mRoutingTable->getPossibleNextHops(pPacket);
  /// a value to compare
  float compare, phi = 0.0;
  /// the index of the element containing the highest pheromone value
  int index = -1;

  /// TODO: That is rather ugly, since we have to iterate over the whole routing table 
  for(unsigned int i = 0; i < list->size(); i++){
    phi = list->get(i)->getPheromoneValue();
    //phi = entry.getPheromoneValue();
    if(compare < phi){
      compare = phi;
      index = i;
    }  
  }

  // todo throw exception
  //if(index < 0){
  //  return NULL; 
  //}

  // create the result 
  NextHop result = NextHop(list->get(index)->getNextHop()->getAddress(), list->get(index)->getNextHop()->getInterface());

  // todo: check ob mir das um die ohren fliegt, mehr als wahrscheinlich
  //return result;
  return result;
}
