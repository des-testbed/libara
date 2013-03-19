/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTConfiguration.h"
#include "omnetpp/OMNeTGate.h"
#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"
#include "SimpleLogger.h"
#include "IPAddressResolver.h"

using namespace ARA;
using namespace ARA::omnetpp;

Logger* OMNeTConfiguration::getLogger(cModule* module) {
    return new SimpleLogger(getHostModule(module)->getName());
}

Configuration OMNeTConfiguration::parseFrom(cModule* module) {
    EvaporationPolicy* evaporationPolicy = check_and_cast<EvaporationPolicy*>(getSubModule(module, "evaporationPolicy", "ARA: the evaporation policy has to be called evaporationPolicy"));
    PathReinforcementPolicy* reinforcementPolicy = check_and_cast<PathReinforcementPolicy*>(getSubModule(module, "pathReinforcementPolicy", "ARA: the routing table has to be called pathReinforcementPolicy"));
    ForwardingPolicy* forwardingPolicy = check_and_cast<ForwardingPolicy*>(getSubModule(module, "forwardingPolicy", "ARA: the forwarding policy has to be called forwardingPolicy"));
    float initialPheromoneValue = module->par("initialPhi").doubleValue();
    int maxNrOfRouteDiscoveryRetries = module->par("nrOfRouteDiscoveryRetries").longValue();
    unsigned int routeDiscoveryTimeoutInMilliSeconds = module->par("routeDiscoveryTimeout").longValue();

    return Configuration(evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPheromoneValue, maxNrOfRouteDiscoveryRetries, routeDiscoveryTimeoutInMilliSeconds);
}

RoutingTable* OMNeTConfiguration::getRoutingTableFrom(cModule* module) {
    return check_and_cast<RoutingTable*>(getSubModule(module, "routingTableStatistics", "ARA: the routing table has to be called routingTableStatistics"));
}


cModule* OMNeTConfiguration::getSubModule(cModule* module, const char* moduleIdentifier, const char* errorMessage) {
    cModule* host = module->getParentModule();
    cModule* subModule = host->getSubmodule(moduleIdentifier);

    if(subModule == NULL){
        throw cRuntimeError(errorMessage);
    }

    return subModule;
}

void OMNeTConfiguration::initializeNetworkInterfacesOf(OMNeTARA* client) {
    double broadCastDelay = client->par("broadCastDelay").doubleValue();
    double uniCastDelay = client->par("uniCastDelay").doubleValue();
    cGate* gateToARP = client->gate("arpOut");

    cModule* host = getHostModule(client);
    client->interfaceTable = IPAddressResolver().findInterfaceTableOf(host);
    if (client->interfaceTable == NULL) {
        throw cRuntimeError("ARA: the interface table has to be called interfaceTable");
    }

    int nrOfInterfaces = client->interfaceTable->getNumInterfaces();
    for (int i=0; i < nrOfInterfaces; i++)         {
        InterfaceEntry* interfaceEntry = client->interfaceTable->getInterface(i);
        if (interfaceEntry->isLoopback() == false) {
            client->addNetworkInterface(new OMNeTGate(client, gateToARP, interfaceEntry, broadCastDelay, uniCastDelay));
        }
    }
}

cModule* OMNeTConfiguration::getHostModule(cModule* module) {
    cModule* parent = module->getParentModule();
    cModule* grandParent = parent->getParentModule();
    return grandParent;
}
