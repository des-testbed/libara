/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTConfiguration.h"
#include "omnetpp/OMNeTGate.h"
#include "omnetpp/OMNeTForwardingPolicy.h"
#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"
#include "IPvXAddressResolver.h"
#include "IInterfaceTable.h"
#include "ModuleAccess.h"

#include <cstring>

OMNETARA_NAMESPACE_BEGIN

OMNeTConfiguration::OMNeTConfiguration(cModule* module, RoutingTable* routingTable, ::ARA::PacketFactory* packetFactory, std::shared_ptr<PacketTrap> packetTrap) {
    // load parameters
    initialPheromoneValue = module->par("initialPhi").doubleValue();
    maxNrOfRouteDiscoveryRetries = module->par("nrOfRouteDiscoveryRetries").longValue();
    maxTTL = module->par("maxTTL").longValue();
    routeDiscoveryTimeoutInMilliSeconds = module->par("routeDiscoveryTimeout").longValue();
    packetDeliveryDelayInMilliSeconds = module->par("packetDeliveryDelay").longValue();
    neighborActivityCheckIntervalInMilliSeconds = module->par("neighborActivityCheckInterval").longValue();
    maxNeighborInactivityTimeInMilliSeconds = module->par("maxNeighborInactivityTime").longValue();
    pantIntervalInMilliSeconds = module->par("pantInterval").longValue();
    previousHopFeatureIsActivated  = module->par("previousHopFeature").boolValue();

    // load child modules
    simpleModule = module;
    evaporationPolicy = ModuleAccess<EvaporationPolicy>("evaporationPolicy").get();
    reinforcementPolicy = ModuleAccess<PathReinforcementPolicy>("reinforcementPolicy").get();

    // configure the routingTable
    if (routingTable == nullptr) {
        //TODO why not set the evaporation policy right in the ctor?
        routingTable = new RoutingTable();
    }
    this->routingTable = routingTable;
    this->routingTable->setEvaporationPolicy(evaporationPolicy);

    logger = new OMNeTLogger(getHostModule()->getFullName());
    setLogLevel(module->par("logLevel").stringValue());

    if (packetFactory == nullptr) {
        packetFactory = new PacketFactory(maxTTL);
    }
    this->packetFactory = packetFactory;

    if (packetTrap == nullptr) {
        packetTrap = std::make_shared<PacketTrap>(routingTable);
    }
    this->packetTrap = packetTrap;
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
    OMNeTForwardingPolicy* forwardingPolicy = ModuleAccess<OMNeTForwardingPolicy>("forwardingPolicy").get();
    forwardingPolicy->setRoutingTable(routingTable);
    return forwardingPolicy;
}

std::shared_ptr<PacketTrap> OMNeTConfiguration::getPacketTrap() {
    return packetTrap;
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
    return routingTable;
}

::ARA::PacketFactory* OMNeTConfiguration::getPacketFactory() {
    return packetFactory;
}

cModule* OMNeTConfiguration::getHostModule() {
    cModule* parent = simpleModule->getParentModule();
    cModule* grandParent = parent->getParentModule();
    return grandParent;
}

bool OMNeTConfiguration::isPreviousHopFeatureActivated() {
    return previousHopFeatureIsActivated;
}

OMNETARA_NAMESPACE_END
