/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEARAConfiguration.h"
#include "ModuleAccess.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTEARAConfiguration::OMNeTEARAConfiguration(cModule* module) : OMNeTConfiguration(module) {
    energyDisseminationTimeout = module->par("helloMessageTimeout").longValue();
}

unsigned int OMNeTEARAConfiguration::getEnergyDisseminationTimeout() const {
    return energyDisseminationTimeout;
}

EnergyAwareRoutingTable* OMNeTEARAConfiguration::getEnergyAwareRoutingTable() {
    EnergyAwareRoutingTable* routingTable = new EnergyAwareRoutingTable();
    routingTable->setEvaporationPolicy(evaporationPolicy);
    return routingTable;
}

OMNETARA_NAMESPACE_END
