/*
 * $FU-Copyright$
 */

#include "RoutingTableEntry.h"

namespace ARA {

RoutingTableEntry::RoutingTableEntry(std::shared_ptr<Address> address, NetworkInterface* interface, float pheromoneValue) {
    nextHop = new NextHop(address, interface);
    this->pheromoneValue = pheromoneValue;
}

RoutingTableEntry::~RoutingTableEntry() {
    delete nextHop;
}

NextHop* RoutingTableEntry::getNextHop() {
    return nextHop;
}

std::shared_ptr<Address> RoutingTableEntry::getAddress() {
    return nextHop->getAddress();
}

NetworkInterface* RoutingTableEntry::getNetworkInterface() {
    return nextHop->getInterface();
}

float RoutingTableEntry::getPheromoneValue() {
    return pheromoneValue;
}

void RoutingTableEntry::setPheromoneValue(float newPheromoneValue) {
    this->pheromoneValue = newPheromoneValue;
}


} /* namespace ARA */
