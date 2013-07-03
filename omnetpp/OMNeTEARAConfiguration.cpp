/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEARAConfiguration.h"
#include "ModuleAccess.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTEARAConfiguration::OMNeTEARAConfiguration(cModule* module) : OMNeTConfiguration(module) {
    maximumEnergyValue = module->par("maximumEnergyValue").longValue();
    influenceOfMinimumEnergyValue = module->par("influenceOfMinimumEnergyValue").longValue();
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

EnergyAwareRoutingTable* OMNeTEARAConfiguration::getEnergyAwareRoutingTable() const {
    EnergyAwareRoutingTable* routingTable = new EnergyAwareRoutingTable();
    routingTable->setEvaporationPolicy(evaporationPolicy);
    return routingTable;
}

ARA::EARAPacketFactory* OMNeTEARAConfiguration::getEARAPacketFactory() const {
    return new EARAPacketFactory(maxTTL);
}

OMNETARA_NAMESPACE_END
