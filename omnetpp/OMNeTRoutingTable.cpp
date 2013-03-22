#include "omnetpp/OMNeTRoutingTable.h"
#include "omnetpp/OMNeTClock.h"
#include "omnetpp/UnorderedMapWatcher.h"
#include "omnetpp/RoutingTableWatcher.h"

namespace ARA {
    namespace omnetpp {
        Define_Module(OMNeTRoutingTable);

        void OMNeTRoutingTable::initialize(){
            new RoutingTableWatcher<RoutingTableEntry>("test",table);
		}

        void OMNeTRoutingTable::handleMessage(cMessage *msg){
            throw cRuntimeError("OMNeTRoutingTable: handleMessage() should never be called!");
		}
    }
}
