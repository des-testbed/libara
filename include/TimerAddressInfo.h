/*
 * $FU-Copyright$
 */

#ifndef PANT_TIMER_INFO_H_
#define PANT_TIMER_INFO_H_

#include "ARAMacros.h"

ARA_NAMESPACE_BEGIN

/**
 * This is a data class which stores the reference to a specific
 * address so it can remain available as context object of a timer.
 *
 * We can't just give it an ordinary AddressPtr instance, because those
 * are no real pointers by nature.
 */
class TimerAddressInfo {
    public:
    TimerAddressInfo(AddressPtr destination) {
            this->destination = destination;
        }

        AddressPtr destination;
};

ARA_NAMESPACE_END

#endif // ROUTE_DISCOVERY_INFO_H_
