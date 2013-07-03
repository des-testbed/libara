/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTConfiguration.h"
#include "omnetpp/OMNeTGate.h"
#include "omnetpp/OMNeTBattery.h"
#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"
#include "IPvXAddressResolver.h"
#include "IInterfaceTable.h"
#include "ModuleAccess.h"

#include <cstring>

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
    pantIntervalInMilliSeconds = module->par("pantInterval").longValue();
    previousHopFeatureIsActivated  = module->par("previousHopFeature").boolValue();

    OMNeTBattery* battery = ModuleAccess<OMNeTBattery>("battery").get();
    maximumBatteryLevel = battery->getCapacity();

    logger = new OMNeTLogger(getHostModule()->getFullName());
    setLogLevel(module->par("logLevel").stringValue());
}

void OMNeTConfiguration::setLogLevel(const char* logLevelParameter) {
    if (strcmp(logLevelParameter, "TRACE") == 0) {
        logger->setLogLevel(Logger::LEVEL_TRACE);
    }
    else if (strcmp(logLevelParameter, "DEBUG") == 0) {
        logger->setLogLevel(Logger::LEVEL_DEBUG);
    }
    else if (strcmp(logLevelParameter, "INFO") == 0) {
        logger->setLogLevel(Logger::LEVEL_INFO);
    }
    else if (strcmp(logLevelParameter, "WARN") == 0) {
        logger->setLogLevel(Logger::LEVEL_WARN);
    }
    else if (strcmp(logLevelParameter, "ERROR") == 0) {
        logger->setLogLevel(Logger::LEVEL_ERROR);
    }
    else if (strcmp(logLevelParameter, "FATAL") == 0) {
        logger->setLogLevel(Logger::LEVEL_FATAL);
    }
    else {
        throw cRuntimeError("Invalid log level '%s'", logLevelParameter);
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

unsigned int OMNeTConfiguration::getPANTIntervalInMilliSeconds() {
    return pantIntervalInMilliSeconds;
}

Logger* OMNeTConfiguration::getLogger() {
    return logger;
}

RoutingTable* OMNeTConfiguration::getRoutingTable() {
    RoutingTable* routingTable = new RoutingTable();
    routingTable->setEvaporationPolicy(evaporationPolicy);
    return routingTable;
}

::ARA::PacketFactory* OMNeTConfiguration::getPacketFactory() {
    return new PacketFactory(maxTTL);
}

cModule* OMNeTConfiguration::getHostModule() {
    cModule* parent = simpleModule->getParentModule();
    cModule* grandParent = parent->getParentModule();
    return grandParent;
}

bool OMNeTConfiguration::isPreviousHopFeatureActivated() {
    return previousHopFeatureIsActivated;
}

double OMNeTConfiguration::getMaximumBatteryLevel() {
    return maximumBatteryLevel;
}
