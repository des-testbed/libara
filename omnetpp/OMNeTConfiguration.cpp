/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTConfiguration.h"
#include "omnetpp/OMNeTGate.h"
#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"
#include "SimpleLogger.h"
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

    logger = new SimpleLogger(getHostModule()->getName());

    broadCastDelay = module->par("broadCastDelay").doubleValue();
    uniCastDelay = module->par("uniCastDelay").doubleValue();
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

Logger* OMNeTConfiguration::getLogger() {
    return logger;
}

double OMNeTConfiguration::getBroadCastDelay() {
    return broadCastDelay;
}

double OMNeTConfiguration::getUniCastDelay() {
    return uniCastDelay;
}

RoutingTable* OMNeTConfiguration::getRoutingTable() {
    RoutingTable* routingTable = ModuleAccess<RoutingTable>("araRoutingTable").get();
    routingTable->setEvaporationPolicy(evaporationPolicy);
    return routingTable;
}

cModule* OMNeTConfiguration::getHostModule() {
    cModule* parent = simpleModule->getParentModule();
    cModule* grandParent = parent->getParentModule();
    return grandParent;
}
