#include "PathReinforcementPolicy.h"

using namespace ARA;

PathReinforcementPolicy::PathReinforcementPolicy(RoutingTable* pRoutingTable):routingTable(pRoutingTable){}

PathReinforcementPolicy::PathReinforcementPolicy(const PathReinforcementPolicy& policy):routingTable(policy.getRoutingTable()){}

void PathReinforcementPolicy::setRoutingTable(RoutingTable* pRoutingTable){
    this->routingTable = pRoutingTable;
}

RoutingTable* PathReinforcementPolicy::getRoutingTable() const{
    return this->routingTable;
}
