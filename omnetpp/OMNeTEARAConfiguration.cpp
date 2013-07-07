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
    peantEnergyThreshold = module->par("peantEnergyThreshold").doubleValue();
    forwardingPolicy = ModuleAccess<OMNeTEARAForwardingPolicy>("forwardingPolicy").get();

    if (influenceOfMinimumEnergyValue < 1) {
        throw cRuntimeError("EARA parameter influenceOfMinimumEnergyValue needs to be >= 1");
    }
    if (peantEnergyThreshold > 1 || (peantEnergyThreshold < 0 && peantEnergyThreshold != -1)) {
        throw cRuntimeError("EARA parameter peantEnergyThreshold needs to be between 0 and 1 (or -1 to be disabled)");
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
    forwardingPolicy->setRoutingTable(routingTable);
    return routingTable;
}

ARA::EARAPacketFactory* OMNeTEARAConfiguration::getEARAPacketFactory() const {
    return new EARAPacketFactory(maxTTL);
}

EARAForwardingPolicy* OMNeTEARAConfiguration::getForwardingPolicy() {
    return forwardingPolicy;
}

float OMNeTEARAConfiguration::getPEANTEnergyThreshold() const {
    return peantEnergyThreshold;
}

OMNETARA_NAMESPACE_END
