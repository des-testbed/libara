/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_ADDRESS_H_
#define _TESTBED_ADDRESS_H_

#include "Address.h"
#include "Testbed.h"

#include <mutex>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>

TESTBED_NAMESPACE_BEGIN

class TestbedAddress : public Address {
    public:
        /**
         * The constructor of the class takes an unsigned char array as an
         * argument and stores it as a ara_address_t (which is typedef to
         * an unsigned char aray of size size 6)
         *
         * @param address The address which should be transformed into a
         * TestbedAddress
         */
        TestbedAddress(u_char* address);

        ~TestbedAddress(){ }

        /**
         * @see Address::toString() const
         */
        virtual std::string toString() const;

        /**
         * @see Address::equals(const Address* otherAddress) const
         */
        virtual bool equals(const Address* otherAddress) const;

        /**
         * @see Address::equals(const std::shared_ptr<Address> otherAddress) const
         */
        virtual bool equals(const std::shared_ptr<Address> otherAddress) const; 

        /**
         * @see Address::getHashValue() const
         */
        virtual size_t getHashValue() const;

        /**
         * Caller must delete returned array, array[0] = MSB
         */
        u_int8_t* getDessertValue() const;

    private:
        /**
         *
         */
        ara_address_t address;

        /**
         * A mutex to protect the members of class TestbedAddress
         */
        //mutable std::mutex addressMutex;
        mutable std::recursive_mutex addressMutex;
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_ADDRESS_H_
