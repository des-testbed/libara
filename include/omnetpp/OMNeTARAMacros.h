/*
 * $FU-Copyright$
 */

#ifndef OMNET_ARA_MACROS_H_
#define OMNET_ARA_MACROS_H_

#include <omnetpp.h>
#include <memory>

#include "Address.h"
typedef std::shared_ptr<ARA::Address> AddressPtr;

#define OMNETARA_NAMESPACE_BEGIN namespace ARA { namespace omnetpp {
#define OMNETARA_NAMESPACE_END }}

#endif /* OMNET_ARA_MACROS_H_ */
