/*
 * $FU-Copyright$
 */
#include "omnetpp/RoutingTableWatcher.h" 

OMNETARA_NAMESPACE_BEGIN
using namespace std;

RoutingTableWatcher::RoutingTableWatcher(unordered_map<AddressPtr, RoutingTableEntryList, AddressHash, AddressPredicate>& table) : cStdVectorWatcherBase("table"), table(table) {}

const char* RoutingTableWatcher::getClassName() const {
    return "RoutingTableWatcher";
}

const char* RoutingTableWatcher::getElemTypeName() const {
    return "RoutingTableEntry";
}

int RoutingTableWatcher::size() const {
    unsigned int tableSize = 0;
    unordered_map<AddressPtr, RoutingTableEntryList, AddressHash, AddressPredicate>::iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        RoutingTableEntryList entryList = iterator->second;
        tableSize += entryList->size();
    }
    return tableSize;
}

string RoutingTableWatcher::at(int wantedPosition) const {
    stringstream out;

    int currentPosition = 0;
    unordered_map<AddressPtr, RoutingTableEntryList, AddressHash, AddressPredicate>::iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        AddressPtr address = iterator->first;
        RoutingTableEntryList entryList = iterator->second;
        for (auto& entry: *entryList) {

            if(currentPosition == wantedPosition) {
                out << "[destination] " << address.get()->toString() << " " << *(entry);
                return out.str();
            }
            else {
                currentPosition++;
            }
        }
    }

    return "Index out of bounds";
}

OMNETARA_NAMESPACE_END
