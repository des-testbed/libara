/*
 * $FU-Copyright$
 */

#ifndef EARA_ROUTINGTABLE_WATCHER_H_
#define EARA_ROUTINGTABLE_WATCHER_H_

#include "OMNeTARAMacros.h"
#include "RoutingTableWatcher.h"
#include "EnergyAwareRoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

class EARARoutingTableWatcher : public RoutingTableWatcher {
    public:
        EARARoutingTableWatcher(EnergyAwareRoutingTable* table);

        virtual std::string at(int i) const;

    protected:
        EnergyAwareRoutingTable* table;
};

OMNETARA_NAMESPACE_END

#endif
