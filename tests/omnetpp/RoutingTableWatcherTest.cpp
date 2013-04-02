/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"

#include "omnetpp/RoutingTableWatcher.h"
#include "RoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"

using namespace ARA;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableWatcherTest) {};

IGNORE_TEST(RoutingTableWatcherTest, watchNormalRoutingTableEntry) {
    unordered_map<AddressPtr, deque<RoutingTableEntry*>*, AddressHash, AddressPredicate> routingTable = unordered_map<AddressPtr, deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>();
    deque<RoutingTableEntry*>* entries = new deque<RoutingTableEntry*>;

    NetworkInterfaceMock interface = NetworkInterfaceMock();
    AddressPtr destination(new AddressMock("192.168.0.1"));
    AddressPtr address(new AddressMock("192.168.0.9"));
    RoutingTableEntry* entry = new RoutingTableEntry(address, &interface, 1.234);
    entries->push_back(entry);

    routingTable[destination] = entries;

    RoutingTableWatcher<RoutingTableEntry> watcher("foo", routingTable);
    STRCMP_EQUAL(watcher.at(0).c_str(), "[destination] 192.168.0.1 [next hop] 192.168.0.9 [phi] 1.234");

    delete entries;
    delete entry;
}
