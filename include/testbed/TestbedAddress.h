/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_ADDRESS_H_
#define _TESTBED_ADDRESS_H_

#include "Address.h"
#include "Testbed.h"

#include "StackTrace.h" 

#include <sstream>
#include <iterator>
#include <algorithm>


TESTBED_NAMESPACE_BEGIN

class TestbedAddress : public Address {
    public:
        TestbedAddress(u_char* address);
        ~TestbedAddress(){
//           printStacktrace();
//           std::cout << "[TestbedAddress] destructor of " << this->toString() << " called " << std::endl;
        }

        virtual std::string toString() const;
        virtual bool equals(const Address* otherAddress) const;
        virtual bool equals(const std::shared_ptr<Address> otherAddress) const; 
        virtual size_t getHashValue() const;

        /**
         * Caller must delete returned array, array[0] = MSB
         */
        u_int8_t* getDessertValue() const;

    private:
        ara_address_t address;
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_ADDRESS_H_
