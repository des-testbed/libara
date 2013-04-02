/*
 * $FU-Copyright$
 */

#ifndef _OMNET_ENERGY_AWARE_ROUTING_TABLE_H_
#define _OMNET_ENERGY_AWARE_ROUTING_TABLE_H_

#include "OMNeTARAMacros.h"
#include "../EnergyAwareRoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

class EnergyAwareRoutingTable : public ::ARA::EnergyAwareRoutingTable, public cSimpleModule {
     protected:
         virtual void initialize();
         virtual void handleMessage(cMessage *msg);
};

OMNETARA_NAMESPACE_END
#endif 
