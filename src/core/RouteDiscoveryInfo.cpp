/*
 * $FU-Copyright$
 */

#include "RouteDiscoveryInfo.h"

ARA_NAMESPACE_BEGIN

RouteDiscoveryInfo::RouteDiscoveryInfo(const Packet* associatedPacket) {
    originalPacket = associatedPacket;
    nrOfRetries = 0;
    this->setType(std::string("RouteDiscoveryInfo"));
}

const Packet* RouteDiscoveryInfo::getPacket(){
    return this->originalPacket;
}

int RouteDiscoveryInfo::getNumberOfRetries(){
    return this->nrOfRetries;
}

void RouteDiscoveryInfo::setNumberOfRetries(int newNrOfRetries){
    this->nrOfRetries = newNrOfRetries;
}

ARA_NAMESPACE_END
