/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEARAConfiguration.h"
#include "ModuleAccess.h"
#include "omnetpp/OMNeTBattery.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTEARAConfiguration::OMNeTEARAConfiguration(cModule* module) : OMNeTConfiguration(module) {
    energyDisseminationTimeout = module->par("helloMessageTimeout").longValue();
    OMNeTBattery* battery = ModuleAccess<OMNeTBattery>("battery").get();
    maximumBatteryLevel = battery->getNominalValue();
}

unsigned int OMNeTEARAConfiguration::getEnergyDisseminationTimeout() const {
    return energyDisseminationTimeout;
}

EnergyAwareRoutingTable* OMNeTEARAConfiguration::getRoutingTable() {
    EnergyAwareRoutingTable* routingTable = new EnergyAwareRoutingTable();
    routingTable->setEvaporationPolicy(evaporationPolicy);
    return routingTable;
}

double OMNeTEARAConfiguration::getMaximumBatteryLevel() {
    return maximumBatteryLevel;
}

OMNETARA_NAMESPACE_END
