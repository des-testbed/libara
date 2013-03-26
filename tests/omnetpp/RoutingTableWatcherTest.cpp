/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"

#include "omnetpp/RoutingTableWatcher.h"
#include "RoutingTableEntry.h"
#include "EnergyAwareRoutingTableEntry.h"
#include "NextHop.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableWatcherTest) {};

IGNORE_TEST(RoutingTableWatcherTest, atRoutingTableEntry) {
    std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate> routingTable = std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>(); 
    std::deque<RoutingTableEntry*> *entries = new std::deque<RoutingTableEntry*>;

    NetworkInterfaceMock interface = NetworkInterfaceMock();
    std::shared_ptr<AddressMock> address(new AddressMock("Foo"));
    RoutingTableEntry* entry = new RoutingTableEntry(address, &interface, 1.234);
    entries->push_back(entry);

    routingTable[address] = entries;

    RoutingTableWatcher<RoutingTableEntry> watcher("foo", routingTable);
    STRCMP_EQUAL(watcher.at(0).c_str(), "[destination] Foo [next hop] Foo [phi] 1.234");

    delete entries;
    delete entry;
}

IGNORE_TEST(RoutingTableWatcherTest, atEnergyAwareRoutingTableEntry) {
    std::unordered_map<AddressPtr, std::deque<EnergyAwareRoutingTableEntry*>*, AddressHash, AddressPredicate> routingTable = std::unordered_map<AddressPtr, std::deque<EnergyAwareRoutingTableEntry*>*, AddressHash, AddressPredicate>(); 
    std::deque<EnergyAwareRoutingTableEntry*> *entries = new std::deque<EnergyAwareRoutingTableEntry*>;

    NetworkInterfaceMock interface = NetworkInterfaceMock();
    std::shared_ptr<AddressMock> address(new AddressMock("Foo"));
    EnergyAwareRoutingTableEntry* entry = new EnergyAwareRoutingTableEntry(address, &interface, 1.234, 5.678);
    entries->push_back(entry);

    routingTable[address] = entries;

    RoutingTableWatcher<EnergyAwareRoutingTableEntry> watcher("foo", routingTable);
    STRCMP_EQUAL(watcher.at(0).c_str(), "[destination] Foo [next hop] Foo [phi] 1.234 [energy] 5.678");

    delete entries;
    delete entry;
}
