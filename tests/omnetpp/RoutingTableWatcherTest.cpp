/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"

#include "omnetpp/RoutingTableWatcher.h"
#include "testAPI/mocks/ARAClientMock.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"

using namespace ARA;
using namespace ARA::omnetpp;
using namespace std;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(RoutingTableWatcherTest) {
    ARAClientMock* client;
    RoutingTable* routingTable;
    NetworkInterfaceMock* interface;

    void setup() {
        client = new ARAClientMock();
        routingTable = client->getRoutingTable();
        interface = client->createNewNetworkInterfaceMock();
    }

    void teardown() {
        delete client;
    }
};

/**
 * Ignored because of the following OMNeT++ warning:
 * ERROR: cStringPool::get("table") invoked outside main() -- please do not use cStringPool from global objects
 * Warning: cStringPool::release(): string "table" released too late, after main() already exited
 */
IGNORE_TEST(RoutingTableWatcherTest, watchNormalRoutingTableEntry) {
    AddressPtr destination1 = AddressPtr(new AddressMock("192.168.0.1"));
    AddressPtr destination2 = AddressPtr(new AddressMock("192.168.0.2"));
    AddressPtr hop1 = AddressPtr(new AddressMock("192.168.0.3"));
    AddressPtr hop2 = AddressPtr(new AddressMock("192.168.0.4"));
    AddressPtr hop3 = AddressPtr(new AddressMock("192.168.0.5"));

    routingTable->update(destination1, hop1, interface, 1.234);
    routingTable->update(destination1, hop2, interface, 5);
    routingTable->update(destination2, hop3, interface, 7.24);

    RoutingTableWatcher watcher(routingTable);
    // we don't care about the order of elements but only for the returned string
    STRCMP_EQUAL("[destination] 192.168.0.2 [next hop] 192.168.0.5 [phi] 7.24", watcher.at(0).c_str());
    STRCMP_EQUAL("[destination] 192.168.0.1 [next hop] 192.168.0.3 [phi] 1.234", watcher.at(1).c_str());
    STRCMP_EQUAL("[destination] 192.168.0.1 [next hop] 192.168.0.4 [phi] 5", watcher.at(2).c_str());
}
