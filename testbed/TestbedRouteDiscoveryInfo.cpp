/*
 * $FU-Copyright$
 */

#include "TestbedRouteDiscoveryInfo.h"

TESTBED_NAMESPACE_BEGIN

TestbedRouteDiscoveryInfo::TestbedRouteDiscoveryInfo(const Packet* associatedPacket) : RouteDiscoveryInfo(associatedPacket) {
    this->setType(std::string("TestbedRouteDiscoveryInfo"));
}

const TestbedPacket* TestbedRouteDiscoveryInfo::getPacket(){
    std::lock_guard<std::recursive_mutex> lock(packetMutex);
    return dynamic_cast<const TestbedPacket*>(RouteDiscoveryInfo::getPacket());
}

int TestbedRouteDiscoveryInfo::getNumberOfRetries(){

    return RouteDiscoveryInfo::getNumberOfRetries();
}

void TestbedRouteDiscoveryInfo::setNumberOfRetries(int newNrOfRetries){

    RouteDiscoveryInfo::setNumberOfRetries(newNrOfRetries);
}

TESTBED_NAMESPACE_END
