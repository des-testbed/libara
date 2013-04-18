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

    RoutingTableEntry* entry = table->getEntryAt(wantedPosition);
    AddressPtr address = entry->getAddress();
    out << "[destination] " << address.get()->toString() << " " << *(entry);
    return out.str();
}

OMNETARA_NAMESPACE_END
