/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEARAConfiguration.h"
#include "ModuleAccess.h"
#include "OMNeTBattery.h"
#include "OMNeTEARAForwardingPolicy.h"
#include "ARANetworkConfigurator.h"

OMNETARA_NAMESPACE_BEGIN

OMNeTEARAConfiguration::OMNeTEARAConfiguration(cModule* module, EnergyAwareRoutingTable* routingTable, EARAPacketFactory* packetFactory) : OMNeTConfiguration(module, routingTable, packetFactory) {
    OMNeTBattery* battery = ModuleAccess<OMNeTBattery>("battery").get();
    this->routingTable = routingTable;
    this->packetFactory = packetFactory;

    //TODO this maximumEnergyValue seems very redundant to the OMNeTConfiguration (check this)
    maximumEnergyValue = battery->getCapacity();

    ARANetworkConfigurator* networkConfig = check_and_cast<ARANetworkConfigurator*>(simulation.getModuleByPath("networkConfigurator"));
    maximumBatteryCapacityInNetwork = networkConfig->getMaximumBatteryCapacityInNetwork();

    influenceOfMinimumEnergyValue = module->par("influenceOfMinimumEnergyValue").longValue();
    routeDiscoveryDelayInMilliSeconds = module->par("routeDiscoveryDelay").longValue();
    peantEnergyThreshold = module->par("peantEnergyThreshold").doubleValue();

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

unsigned int OMNeTEARAConfiguration::getMaximumBatteryCapacityInNetwork() const {
    return maximumBatteryCapacityInNetwork;
}

float OMNeTEARAConfiguration::getInfluenceOfMinimumEnergyValue() const {
    return influenceOfMinimumEnergyValue;
}

unsigned int OMNeTEARAConfiguration::getRouteDiscoveryDelayInMilliSeconds() const {
    return routeDiscoveryDelayInMilliSeconds;
}

EnergyAwareRoutingTable* OMNeTEARAConfiguration::getEnergyAwareRoutingTable() const {
    return routingTable;
}

ARA::EARAPacketFactory* OMNeTEARAConfiguration::getEARAPacketFactory() const {
    return packetFactory;
}

EARAForwardingPolicy* OMNeTEARAConfiguration::getForwardingPolicy() {
    OMNeTEARAForwardingPolicy* forwardingPolicy = ModuleAccess<OMNeTEARAForwardingPolicy>("forwardingPolicy").get();
    forwardingPolicy->setRoutingTable(routingTable);
    return forwardingPolicy;
}

float OMNeTEARAConfiguration::getPEANTEnergyThreshold() const {
    return peantEnergyThreshold;
}

OMNETARA_NAMESPACE_END
