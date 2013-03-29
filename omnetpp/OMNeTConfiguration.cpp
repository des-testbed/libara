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
#include "ModuleAccess.h"

using namespace ARA;
using namespace ARA::omnetpp;

OMNeTConfiguration::OMNeTConfiguration(cModule* module) : Configuration(
        evaporationPolicy = ModuleAccess<EvaporationPolicy>("evaporationPolicy").get(),
        reinforcementPolicy = ModuleAccess<PathReinforcementPolicy>("pathReinforcementPolicy").get(),
        forwardingPolicy = ModuleAccess<ForwardingPolicy>("forwardingPolicy").get(),
        initialPheromoneValue = module->par("initialPhi").doubleValue(),
        maxNrOfRouteDiscoveryRetries = module->par("nrOfRouteDiscoveryRetries").longValue(),
        routeDiscoveryTimeoutInMilliSeconds = module->par("routeDiscoveryTimeout").longValue()
    ) {

    logger = new SimpleLogger(getHostModule(module)->getName());

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

cModule* OMNeTConfiguration::getHostModule(cModule* module) {
    cModule* parent = module->getParentModule();
    cModule* grandParent = parent->getParentModule();
    return grandParent;
}
