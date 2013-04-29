/*
 * $FU-Copyright$
 */

#ifndef OMNET_ARA_MACROS_H_
#define OMNET_ARA_MACROS_H_

#include <omnetpp.h>
#include <memory>

#define OMNETARA_NAMESPACE_BEGIN namespace ARA { namespace omnetpp {
#define OMNETARA_NAMESPACE_END }}

#include "Address.h"
#include "omnetpp/OMNeTAddress.h"

class OMNeTAddress;

OMNETARA_NAMESPACE_BEGIN
typedef std::shared_ptr<Address> AddressPtr;
typedef std::shared_ptr<OMNeTAddress> OMNeTAddressPtr;
OMNETARA_NAMESPACE_END

#endif /* OMNET_ARA_MACROS_H_ */
