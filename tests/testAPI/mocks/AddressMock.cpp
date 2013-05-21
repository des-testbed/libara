/*
 * $FU-Copyright$
 */

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

std::string AddressMock::toString() const {
    return address;
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
    if(otherAddressMock == nullptr) {
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
