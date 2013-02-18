/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <stddef.h>
#include <memory>

namespace ARA {

/**
 * This represents an arbitrary network address.
 */
class Address {
public:
    virtual ~Address() {}

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
