#ifndef _OMNET_ROUTING_TABLE_H_
#define _OMNET_ROUTING_TABLE_H_

#include <omnetpp.h>

#include "RoutingTable.h"
#include "RoutingTableEntry.h"

namespace ARA {
    namespace omnetpp {
        /**
         *
         *
         *
         */
        class OMNeTRoutingTable : public ARA::RoutingTable, public cObject {
             public:
                 OMNeTRoutingTable(){};
                 OMNeTRoutingTable(const OMNeTRoutingTable& other);
                 virtual ~OMNeTRoutingTable(){};

                 virtual OMNeTRoutingTable *dup() const {return new OMNeTRoutingTable(*this);};
                 virtual const char *getClassName() const;
                 virtual std::string info() const;
        };
    } 
} 
#endif 
