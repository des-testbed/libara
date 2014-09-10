/*
 * $FU-Copyright$
 */

#include "TimerAddressInfo.h"

ARA_NAMESPACE_BEGIN

TimerAddressInfo::TimerAddressInfo(AddressPtr destination) {
    this->destination = destination;
    this->setType(std::string("TimerAddressInfo"));
}

std::shared_ptr<Address> TimerAddressInfo::getDestination() {
    return destination;
}

ARA_NAMESPACE_END
