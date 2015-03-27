/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_H_
#define _TESTBED_H_

#include "ARAMacros.h"

#include "CLibs.h"

#include <memory>

#define TESTBED_NAMESPACE_BEGIN namespace ARA { namespace testbed {
#define TESTBED_NAMESPACE_END }}

typedef u_char ara_address_t[ETHER_ADDR_LEN];

/**
 * Types used for ARA's DES-SERT extensions (from DES-ARA)
 */
enum _ARA_EXT_TYPES {
    /// forward ant (FANT)
    ARA_EXT_FANT = DESSERT_EXT_USER,    
    /// backward ant (FANT)
    ARA_EXT_BANT,                     
};


#include "TestbedAddress.h"
class TestbedAddress;

TESTBED_NAMESPACE_BEGIN

typedef std::shared_ptr<TestbedAddress> TestbedAddressPtr;

#define DESSERT_LOCAL_ADDRESS dessert_l25_defsrc
#define DESSERT_BROADCAST_ADDRESS ether_broadcast

TESTBED_NAMESPACE_END

#endif 
