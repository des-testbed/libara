/*
 * $FU-Copyright$
 */

#ifndef __ROUTINGTABLE_WATCH_H
#define __ROUTINGTABLE_WATCH_H

#include "cwatch.h"
#include "cstlwatch.h" 
#include "cownedobject.h"

#include <unordered_map>
#include "RoutingTable.h" 
#include "RoutingTableEntry.h" 

NAMESPACE_BEGIN

class RoutingTableWatcher : public cStdVectorWatcherBase {
    protected:
        std::unordered_map<std::shared_ptr<ARA::Address>, std::deque<ARA::RoutingTableEntry*>*, ARA::AddressHash, ARA::AddressPredicate>& m;
        mutable typename std::unordered_map<std::shared_ptr<ARA::Address>, std::deque<ARA::RoutingTableEntry*>*, ARA::AddressHash, ARA::AddressPredicate>::iterator it;
        mutable int itPos;
        std::string classname;

    public:
        RoutingTableWatcher(const char *name, std::unordered_map<std::shared_ptr<ARA::Address>, std::deque<ARA::RoutingTableEntry*>*, ARA::AddressHash, ARA::AddressPredicate>& var);

        const char *getClassName() const;
        virtual const char *getElemTypeName() const;
        virtual int size() const;
        virtual std::string at(int i) const;
        virtual std::string atIt() const;
};

NAMESPACE_END

#endif
