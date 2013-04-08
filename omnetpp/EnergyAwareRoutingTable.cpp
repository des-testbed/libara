#include "omnetpp/EnergyAwareRoutingTable.h"
#include "omnetpp/UnorderedMapWatcher.h"
#include "omnetpp/RoutingTableWatcher.h"
#include "RoutingTableEntry.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(EnergyAwareRoutingTable);

void EnergyAwareRoutingTable::initialize(){
    new RoutingTableWatcher(table);
}

void EnergyAwareRoutingTable::handleMessage(cMessage *msg){
    throw cRuntimeError("EnergyAwareRoutingTable: handleMessage() should never be called!");
}

OMNETARA_NAMESPACE_END
