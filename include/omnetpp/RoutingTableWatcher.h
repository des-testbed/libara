/*
 * $FU-Copyright$
 */

#ifndef __ROUTINGTABLE_WATCH_H
#define __ROUTINGTABLE_WATCH_H

#include "OMNeTARAMacros.h"
#include "RoutingTableEntry.h"

#include "cwatch.h"
#include "cstlwatch.h" 
#include "cownedobject.h"

#include <deque>
#include <unordered_map>
#include "Address.h" 

OMNETARA_NAMESPACE_BEGIN

#define RoutingTableEntryList std::deque<RoutingTableEntry*>*

class RoutingTableWatcher : public cStdVectorWatcherBase {
    public:
        RoutingTableWatcher(std::unordered_map<AddressPtr, RoutingTableEntryList, AddressHash, AddressPredicate>& table);

        const char* getClassName() const;
        virtual const char* getElemTypeName() const;
        virtual int size() const;
        virtual std::string at(int i) const;

    protected:
        std::unordered_map<AddressPtr, RoutingTableEntryList, AddressHash, AddressPredicate>& table;
};

OMNETARA_NAMESPACE_END

#endif
