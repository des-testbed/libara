/*
 * $FU-Copyright$
 */

#ifndef TRAFFIC_CONTROL_INFO_H_
#define TRAFFIC_CONTROL_INFO_H_

#include "OMNeTARAMacros.h"

OMNETARA_NAMESPACE_BEGIN

class TrafficControlInfo : public cObject {
public:
    TrafficControlInfo();

    void setDestination(const char* destinationIPv4Address);
    void setHopCount(int hopCount);

    AddressPtr getDestination() const;
    int getHopCount() const;

private:
    AddressPtr destination;
    int hopCount;
};

OMNETARA_NAMESPACE_END

#endif /* TRAFFIC_CONTROL_INFO_H_ */
