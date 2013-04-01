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

OMNeTConfiguration::OMNeTConfiguration(cModule* module) : Configuration(
        evaporationPolicy = ModuleAccess<EvaporationPolicy>("evaporationPolicy").get(),
        reinforcementPolicy = ModuleAccess<PathReinforcementPolicy>("reinforcementPolicy").get(),
        forwardingPolicy = ModuleAccess<ForwardingPolicy>("forwardingPolicy").get(),
        initialPheromoneValue = module->par("initialPhi").doubleValue(),
        maxNrOfRouteDiscoveryRetries = module->par("nrOfRouteDiscoveryRetries").longValue(),
        routeDiscoveryTimeoutInMilliSeconds = module->par("routeDiscoveryTimeout").longValue()
    ) {
    this->module = module;
    logger = new SimpleLogger(getHostModule()->getName());

    routingTable = ModuleAccess<RoutingTable>("araRoutingTable").get();
    routingTable->setEvaporationPolicy(evaporationPolicy);

    broadCastDelay = module->par("broadCastDelay").doubleValue();
    uniCastDelay = module->par("uniCastDelay").doubleValue();
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
    return routingTable;
}

cModule* OMNeTConfiguration::getHostModule() {
    cModule* parent = module->getParentModule();
    cModule* grandParent = parent->getParentModule();
    return grandParent;
}
