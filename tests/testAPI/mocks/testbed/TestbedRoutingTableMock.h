/*
 * $FU-Copyright$
 */

#ifndef TESTBED_ROUTING_TABLE_MOCK_H_
#define TESTBED_ROUTING_TABLE_MOCK_H_

#include "Testbed.h"
#include "TestbedRoutingTable.h"

TESTBED_NAMESPACE_BEGIN

class TestbedRoutingTableMock : public TestbedRoutingTable {
    public:
        RoutingTableEntryList getPossibleNextHops(AddressPtr address);
};

TESTBED_NAMESPACE_END

#endif /* TESTBED_ROUTING_TABLE_MOCK_H_ */
