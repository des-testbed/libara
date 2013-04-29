/*
 * $FU-Copyright$
 */

#ifndef ROUTING_TABLE_MOCK_H_
#define ROUTING_TABLE_MOCK_H_

#include "RoutingTable.h"
#include "ARAMacros.h"

namespace ARA {

class RoutingTableMock: public RoutingTable {
public:

    RoutingTableEntryList getPossibleNextHops(AddressPtr address);

};

} /* namespace ARA */
#endif /* ROUTING_TABLE_MOCK_H_ */
