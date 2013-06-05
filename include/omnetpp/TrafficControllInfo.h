/*
 * $FU-Copyright$
 */

#ifndef TRAFFIC_CONTROL_INFO_H_
#define TRAFFIC_CONTROL_INFO_H_

#include "OMNeTARAMacros.h"

OMNETARA_NAMESPACE_BEGIN

class TrafficControlInfo : public cObject {
public:
    TrafficControlInfo(const char* destinationIPv4Address);
    AddressPtr getDestination() const;

private:
    AddressPtr destination;
};

OMNETARA_NAMESPACE_END

#endif /* TRAFFIC_CONTROL_INFO_H_ */
