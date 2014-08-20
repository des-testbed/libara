/*
 * $FU-Copyright$
 */

#ifndef TIMER_ADDRESS_INFO_H_
#define TIMER_ADDRESS_INFO_H_

#include "ARAMacros.h"
#include "Context.h"

ARA_NAMESPACE_BEGIN

/**
 * This is a data class which stores the reference to a specific
 * address so it can remain available as context object of a timer.
 *
 * We can't just give it an ordinary AddressPtr instance, because those
 * are no real pointers by nature.
 */
class TimerAddressInfo : public Context {
    public:
        TimerAddressInfo(AddressPtr destination);

        /**
         * The method returns an shared_ptr of type address.
         * @return A shared_ptr of type Address
         */
        std::shared_ptr<Address> getDestination();

    private:
        AddressPtr destination;
};

ARA_NAMESPACE_END

#endif // TIMER_ADDRESS_INFO_H_
