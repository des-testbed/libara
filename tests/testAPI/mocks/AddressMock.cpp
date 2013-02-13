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

#include "AddressMock.h"
#include <hash_fun.h>

using namespace std;

namespace ARA {

AddressMock::AddressMock() {
    this->address = string("Foo");
}

AddressMock::AddressMock(const std::string name) {
    this->address = name;
}

bool AddressMock::equals(const Address* otherAddress) const {
    const AddressMock* otherAddressMock = dynamic_cast<const AddressMock*>(otherAddress);
    if(otherAddressMock == NULL) {
        return false;
    }
    else {
        return this->address.compare(otherAddressMock->address) == 0;
    }
}

bool AddressMock::equals(const shared_ptr<Address> otherAddress) const {
    shared_ptr<AddressMock> otherAddressMock (dynamic_pointer_cast<AddressMock>(otherAddress));
    if(otherAddressMock == NULL) {
        return false;
    }
    else {
        return this->address.compare(otherAddressMock->address) == 0;
    }
}

size_t AddressMock::getHashValue() const {
    __gnu_cxx::hash<const char*> hashFunction;
    return hashFunction(address.c_str());
}

string AddressMock::getAddress() {
    return address;
}

Address* AddressMock::clone() {
    AddressMock* clone = new AddressMock(this->address);
    return clone;
}

} /* namespace ARA */
