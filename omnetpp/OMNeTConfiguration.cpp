/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTConfiguration.h"
#include "omnetpp/OMNeTGate.h"
#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"
#include "IPvXAddressResolver.h"
#include "IInterfaceTable.h"
#include "ModuleAccess.h"

using namespace ARA;
using namespace ARA::omnetpp;

OMNeTConfiguration::OMNeTConfiguration(cModule* module) {
    simpleModule = module;
    evaporationPolicy = ModuleAccess<EvaporationPolicy>("evaporationPolicy").get();
    reinforcementPolicy = ModuleAccess<PathReinforcementPolicy>("reinforcementPolicy").get();
    forwardingPolicy = ModuleAccess<ForwardingPolicy>("forwardingPolicy").get();
    initialPheromoneValue = module->par("initialPhi").doubleValue();
    maxNrOfRouteDiscoveryRetries = module->par("nrOfRouteDiscoveryRetries").longValue();
    maxTTL = module->par("maxTTL").longValue();
    routeDiscoveryTimeoutInMilliSeconds = module->par("routeDiscoveryTimeout").longValue();
    packetDeliveryDelayInMilliSeconds = module->par("packetDeliveryDelay").longValue();
    neighborActivityCheckIntervalInMilliSeconds = module->par("neighborActivityCheckInterval").longValue();
    maxNeighborInactivityTimeInMilliSeconds = module->par("maxNeighborInactivityTime").longValue();

    logger = new OMNeTLogger(getHostModule()->getFullName());

    //FIXME make this more generic via a module parameter
    if(strcmp(getHostModule()->getFullName(), "node[1]") == 0) {
        logger->setLogLevel(Logger::LEVEL_TRACE);
    }
}

EvaporationPolicy* OMNeTConfiguration::getEvaporationPolicy() {
    return evaporationPolicy;
}

PathReinforcementPolicy* OMNeTConfiguration::getReinforcementPolicy() {
    return reinforcementPolicy;
}

ForwardingPolicy* OMNeTConfiguration::getForwardingPolicy() {
    return forwardingPolicy;
}

float OMNeTConfiguration::getInitialPheromoneValue() {
    return initialPheromoneValue;
}

int OMNeTConfiguration::getMaxNrOfRouteDiscoveryRetries() {
    return maxNrOfRouteDiscoveryRetries;
}

int OMNeTConfiguration::getMaxTTL() {
    return maxTTL;
}

unsigned int OMNeTConfiguration::getRouteDiscoveryTimeoutInMilliSeconds() {
    return routeDiscoveryTimeoutInMilliSeconds;
}

unsigned int OMNeTConfiguration::getPacketDeliveryDelayInMilliSeconds() {
    return packetDeliveryDelayInMilliSeconds;
}

unsigned int OMNeTConfiguration::getNeighborActivityCheckIntervalInMilliSeconds() {
    return neighborActivityCheckIntervalInMilliSeconds;
}

unsigned int OMNeTConfiguration::getMaxNeighborInactivityTimeInMilliSeconds() {
    return maxNeighborInactivityTimeInMilliSeconds;
}

Logger* OMNeTConfiguration::getLogger() {
    return logger;
}

RoutingTable* OMNeTConfiguration::getRoutingTable() {
    RoutingTable* routingTable = new RoutingTable();
    routingTable->setEvaporationPolicy(evaporationPolicy);
    return routingTable;
}

cModule* OMNeTConfiguration::getHostModule() {
    cModule* parent = simpleModule->getParentModule();
    cModule* grandParent = parent->getParentModule();
    return grandParent;
}
