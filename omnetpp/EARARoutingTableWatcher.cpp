/*
 * $FU-Copyright$
 */
#include "omnetpp/EARARoutingTableWatcher.h"

OMNETARA_NAMESPACE_BEGIN
using namespace std;

EARARoutingTableWatcher::EARARoutingTableWatcher(EnergyAwareRoutingTable* table) : RoutingTableWatcher(table) {
    this->table = table;
}

string EARARoutingTableWatcher::at(int wantedPosition) const {
    stringstream out;

    EARARoutingTableEntryTupel entryTupel = table->getEntryAt(wantedPosition);
    AddressPtr destination = entryTupel.destination;
    EARARoutingTableEntry* entry = entryTupel.entry;
    out << "[destination] " << destination->toString() << " " << *entry;
    return out.str();
}

OMNETARA_NAMESPACE_END
