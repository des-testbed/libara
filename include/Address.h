/*
 * $FU-Copyright$
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <memory>
#include <sstream>
#include <stddef.h>
#include <iostream>
#include <string>

namespace ARA {

/**
 * This represents an arbitrary network address.
 */
class Address {
public:
    virtual ~Address() {}

    /**
     * Returns a null-terminated textual representation of this address.
     * This is only used for logging purpose.
     */
    virtual std::string toString() const = 0;

    virtual bool equals(const Address* otherAddress) const = 0;
    virtual bool equals(const std::shared_ptr<Address> otherAddress) const = 0; // FIXME 2 abstract equals definitions is not necessary
    virtual size_t getHashValue() const = 0;
    virtual Address* clone() = 0;

    bool operator==(const Address& otherAddress) const {
            return this->equals(&otherAddress);
        }

    bool operator==(const std::shared_ptr<Address> otherAddress) const {
        return this->equals(otherAddress);
    }
    
    friend std::ostream& operator<<(std::ostream& out, Address& address){
        out << address.toString(); 
        return out;
    }

};

/**
 * This Functor is needed for std::unordered_map (hashmap implementation)
 */
struct AddressHash {
    size_t operator()(std::shared_ptr<Address> address) const {
        return address->getHashValue();
    }
};

/**
 * This Functor is needed for std::unordered_map (hashmap implementation)
 */
struct AddressPredicate {
    size_t operator()(std::shared_ptr<Address> address1, std::shared_ptr<Address> address2) const {
        return address1->equals(address2);
    }
};

} /* namespace ARA */
#endif /* ADDRESS_H_ */
