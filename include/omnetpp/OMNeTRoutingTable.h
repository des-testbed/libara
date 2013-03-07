/*
 * $FU-Copyright$
 */

#ifndef _OMNET_ROUTING_TABLE_H_
#define _OMNET_ROUTING_TABLE_H_

#include <omnetpp.h>

#include "RoutingTable.h"
#include "RoutingTableEntry.h"
#include "OMNeTClock.h"

namespace ARA {
    namespace omnetpp {
        /**
         *
         *
         *
         */
        class OMNeTRoutingTable : public ARA::RoutingTable, public cSimpleModule {
             public:
                 OMNeTRoutingTable() : RoutingTable() {};
                 OMNeTRoutingTable(const OMNeTRoutingTable& other); //TODO do we really need the copy constructor?
                 virtual ~OMNeTRoutingTable(){}; //TODO do we really need this destructor?
                 virtual OMNeTRoutingTable *dup() const {return new OMNeTRoutingTable(*this);};

             protected:
                 virtual void initialize();
                 virtual void handleMessage(cMessage *msg);
        };
    } 
} 
#endif 
