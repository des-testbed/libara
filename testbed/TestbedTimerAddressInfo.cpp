/*
 * $FU-Copyright$
 */

#include "TestbedTimerAddressInfo.h"

TESTBED_NAMESPACE_BEGIN

TestbedTimerAddressInfo::TestbedTimerAddressInfo(std::shared_ptr<Address> destination) : TimerAddressInfo(destination) { }

std::shared_ptr<TestbedAddress> TestbedTimerAddressInfo::getDestination() {
    try {
        std::lock_guard<std::mutex> lock(addressMutex);
    } catch (const std::system_error& error) {
        // DEBUG:
        std::cerr << "[TestbedTimerAddressInfo] caught system_error" << std::endl;
        std::cerr << "Error:    " << error.what() << std::endl;
        std::cerr << "Code:     " << error.code().value() << std::endl;
        std::cerr << "Category: " << error.code().category().name() << std::endl;
        std::cerr << "Message:  " << error.code().message() << std::endl;
    }

    std::shared_ptr<TestbedAddress> address = std::dynamic_pointer_cast<TestbedAddress>(TimerAddressInfo::getDestination());
    return address;
}

TESTBED_NAMESPACE_END
