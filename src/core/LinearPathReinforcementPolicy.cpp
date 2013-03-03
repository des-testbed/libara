#include "LinearPathReinforcementPolicy.h"

using namespace ARA;

LinearPathReinforcementPolicy::LinearPathReinforcementPolicy(RoutingTable* pRoutingTable, float pDeltaPhi): deltaPhi(pDeltaPhi){
    this->setRoutingTable(pRoutingTable);
}

LinearPathReinforcementPolicy::LinearPathReinforcementPolicy(const LinearPathReinforcementPolicy& policy){
    this->setRoutingTable(policy.getRoutingTable());
}

void LinearPathReinforcementPolicy::update(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface){
     float phi = this->routingTable->getPheromoneValue(destination, nextHop, interface);
     phi += this->deltaPhi; 
     this->routingTable->update(destination, nextHop, interface, phi);
}

float LinearPathReinforcementPolicy::getDeltaPhi(){
    return this->deltaPhi;
}

void LinearPathReinforcementPolicy::setDeltaPhi(float pDeltaPhi){
    this->deltaPhi = pDeltaPhi;
}
