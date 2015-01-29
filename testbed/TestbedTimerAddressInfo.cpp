/*
 * $FU-Copyright$
 */

#include "TestbedTimerAddressInfo.h"

TESTBED_NAMESPACE_BEGIN

TestbedTimerAddressInfo::TestbedTimerAddressInfo(std::shared_ptr<Address> destination) : TimerAddressInfo(destination) { 
    try {
        logger = spdlog::get("file_logger");
    } catch (const spdlog::spdlog_ex& exception) {
        std::cerr << "getting file logger failed: " << exception.what() << std::endl;
    }
}

std::shared_ptr<TestbedAddress> TestbedTimerAddressInfo::getDestination() {
    try {
        std::lock_guard<std::mutex> lock(addressMutex);
    } catch (const std::system_error& error) {
        logger->error() << "caught system_error:";
        logger->error() << " Error:    " << error.what();
        logger->error() << " Code:     " << error.code().value();
        logger->error() << " Category: " << error.code().category().name();
        logger->error() << " Message:  " << error.code().message();
    }

    std::shared_ptr<TestbedAddress> address = std::dynamic_pointer_cast<TestbedAddress>(TimerAddressInfo::getDestination());
    return address;
}

TESTBED_NAMESPACE_END
