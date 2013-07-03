/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEARAConfiguration.h"
#include "ModuleAccess.h"
#include "OMNeTEARAForwardingPolicy.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTEARAConfiguration::OMNeTEARAConfiguration(cModule* module) : OMNeTConfiguration(module) {
    maximumEnergyValue = module->par("maximumEnergyValue").longValue();
    influenceOfMinimumEnergyValue = module->par("influenceOfMinimumEnergyValue").longValue();
    routeDiscoveryDelayInMilliSeconds = module->par("routeDiscoveryDelay").longValue();
    if (influenceOfMinimumEnergyValue < 1) {
        throw cRuntimeError("EARA parameter influenceOfMinimumEnergyValue needs to be >= 1");
    }
}

unsigned int OMNeTEARAConfiguration::getMaximumEnergyValue() const {
    return maximumEnergyValue;
}

float OMNeTEARAConfiguration::getInfluenceOfMinimumEnergyValue() const {
    return influenceOfMinimumEnergyValue;
}

unsigned int OMNeTEARAConfiguration::getRouteDiscoveryDelayInMilliSeconds() const {
    return routeDiscoveryDelayInMilliSeconds;
}

EnergyAwareRoutingTable* OMNeTEARAConfiguration::getEnergyAwareRoutingTable() const {
    EnergyAwareRoutingTable* routingTable = new EnergyAwareRoutingTable();
    routingTable->setEvaporationPolicy(evaporationPolicy);
    ((OMNeTEARAForwardingPolicy*)forwardingPolicy)->setRoutingTable(routingTable);
    return routingTable;
}

ARA::EARAPacketFactory* OMNeTEARAConfiguration::getEARAPacketFactory() const {
    return new EARAPacketFactory(maxTTL);
}

OMNETARA_NAMESPACE_END
