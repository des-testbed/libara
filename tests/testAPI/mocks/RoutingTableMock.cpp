/*
 * $FU-Copyright$
 */

#include "RoutingTableMock.h"

namespace ARA {

RoutingTableEntryList RoutingTableMock::getPossibleNextHops(AddressPtr destination) {
    return *(table[destination]);
}

} /* namespace ARA */
