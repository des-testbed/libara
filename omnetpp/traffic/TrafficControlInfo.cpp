/*
 * $FU-Copyright$
 */

#include "omnetpp/TrafficControllInfo.h"
#include "omnetpp/OMNeTAddress.h"

#include "IPv4Address.h"

OMNETARA_NAMESPACE_BEGIN

TrafficControlInfo::TrafficControlInfo() {
    destination = nullptr;
    hopCount = 0;
}

void TrafficControlInfo::setDestination(const char* destinationIPv4Address) {
    IPv4Address destinationAddress = IPv4Address(destinationIPv4Address);
    destination = AddressPtr(new OMNeTAddress(destinationAddress));
}

void TrafficControlInfo::setHopCount(int hopCount) {
    this->hopCount = hopCount;
}

int TrafficControlInfo::getHopCount() const {
    ASSERT(hopCount > 0);
    return hopCount;
}

AddressPtr TrafficControlInfo::getDestination() const {
    ASSERT(destination != nullptr);
    return destination;
}

OMNETARA_NAMESPACE_END
