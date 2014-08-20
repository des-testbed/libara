/*
 * $FU-Copyright$
 */

#ifndef TESTBED_TIMER_ADDRESS_INFO_H_
#define TESTBED_TIMER_ADDRESS_INFO_H_

#include "Testbed.h"
#include "TestbedAddress.h"
#include "TimerAddressInfo.h"

#include <mutex>

TESTBED_NAMESPACE_BEGIN

/**
 * This class stores information associated with a running timer object.
 * Typically, this is an address object. However, this class simply extends
 * the TimerAddressInfo by means of a mutex which protexts the information
 * hold in the class.
 */
class TestbedTimerAddressInfo : public TimerAddressInfo {
    public:
        TestbedTimerAddressInfo(std::shared_ptr<TestbedAddress> destination);

        //std::shared_ptr<TestbedAddress> getAddress();
        std::shared_ptr<Address> getDestination();

    private:
        std::mutex addressMutex;
};

TESTBED_NAMESPACE_END

#endif
