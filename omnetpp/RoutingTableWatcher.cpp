/*
 * $FU-Copyright$
 */
#include "omnetpp/RoutingTableWatcher.h" 

OMNETARA_NAMESPACE_BEGIN
using namespace std;

RoutingTableWatcher::RoutingTableWatcher(RoutingTable* table) : cStdVectorWatcherBase("routingTable"), table(table) {}

const char* RoutingTableWatcher::getClassName() const {
    return "RoutingTable*";
}

const char* RoutingTableWatcher::getElemTypeName() const {
    return "RoutingTableEntry";
}

int RoutingTableWatcher::size() const {
    return table->getTotalNumberOfEntries();
}

string RoutingTableWatcher::at(int wantedPosition) const {
    stringstream out;

    RoutingTableEntryTupel entryTupel = table->getEntryAt(wantedPosition);
    AddressPtr destination = entryTupel.destination;
    RoutingTableEntry* entry = entryTupel.entry;
    out << "[destination] " << destination->toString() << " " << *entry;
    return out.str();
}

OMNETARA_NAMESPACE_END
