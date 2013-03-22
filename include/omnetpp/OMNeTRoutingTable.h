/*
 * $FU-Copyright$
 */

#ifndef _OMNET_ROUTING_TABLE_H_
#define _OMNET_ROUTING_TABLE_H_

#include <omnetpp.h>

#include "RoutingTable.h"

namespace ARA {
    namespace omnetpp {
        class OMNeTRoutingTable : public ARA::RoutingTable, public cSimpleModule {
             protected:
                 virtual void initialize();
                 virtual void handleMessage(cMessage *msg);
        };
    } 
} 
#endif 
