/*
 * $FU-Copyright$
 */

#ifndef OMNET_ARA_MACROS_H_
#define OMNET_ARA_MACROS_H_

#include "ARAMacros.h"
#include <omnetpp.h>

#define OMNETARA_NAMESPACE_BEGIN namespace ARA { namespace omnetpp {
#define OMNETARA_NAMESPACE_END }}

#include "omnetpp/OMNeTAddress.h"

class OMNeTAddress;

OMNETARA_NAMESPACE_BEGIN
typedef std::shared_ptr<OMNeTAddress> OMNeTAddressPtr;
OMNETARA_NAMESPACE_END

#endif /* OMNET_ARA_MACROS_H_ */
