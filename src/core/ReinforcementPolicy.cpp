#include "ReinforcementPolicy.h"

using namespace ARA;

ReinforcementPolicy::ReinforcementPolicy(RoutingTable* pRoutingTable):routingTable(pRoutingTable){}

ReinforcementPolicy::ReinforcementPolicy(const ReinforcementPolicy& policy):routingTable(policy.getRoutingTable()){}

void ReinforcementPolicy::setRoutingTable(RoutingTable* pRoutingTable){
    this->routingTable = pRoutingTable;
}

RoutingTable* ReinforcementPolicy::getRoutingTable() const{
    return this->routingTable;
}
