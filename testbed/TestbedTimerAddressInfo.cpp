/*
 * $FU-Copyright$
 */

#include "TestbedTimerAddressInfo.h"

TESTBED_NAMESPACE_BEGIN

TestbedTimerAddressInfo::TestbedTimerAddressInfo(std::shared_ptr<TestbedAddress> destination) : TimerAddressInfo(destination) { }

std::shared_ptr<TestbedAddress> TestbedTimerAddressInfo::getAddress() {
    std::lock_guard<std::mutex> lock(addressMutex);
    std::shared_ptr<TestbedAddress> result = std::dynamic_pointer_cast<TestbedAddress>(destination);
    return result;
}

TESTBED_NAMESPACE_END
