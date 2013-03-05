#include "RouteDiscoveryTimer.h"

using namespace ARA;

bool RouteDiscoveryTimer::isRunning(std::shared_ptr<Address> address){
    return table.find(address) != table.end();
}
