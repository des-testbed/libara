/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_ADDRESS_H_
#define _TESTBED_ADDRESS_H_

#include "Address.h"
#include "Testbed.h"

TESTBED_NAMESPACE_BEGIN

class TestbedAddress : public Address {
    public:
        TestbedAddress(u_char* address);
        virtual std::string toString() const;
        virtual bool equals(const Address* otherAddress) const;
        virtual bool equals(const std::shared_ptr<Address> otherAddress) const; 
        virtual size_t getHashValue() const;

    private:
        ara_address_t address;
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_ADDRESS_H_
