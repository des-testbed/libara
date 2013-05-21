/*
 * $FU-Copyright$
 */

#ifndef __ROUTINGTABLE_WATCH_H
#define __ROUTINGTABLE_WATCH_H

#include "OMNeTARAMacros.h"
#include "RoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

class RoutingTableWatcher : public cStdVectorWatcherBase {
    public:
        RoutingTableWatcher(RoutingTable* table);

        const char* getClassName() const;
        virtual const char* getElemTypeName() const;
        virtual int size() const;
        virtual std::string at(int i) const;

    protected:
        RoutingTable* table;
};

OMNETARA_NAMESPACE_END

#endif
