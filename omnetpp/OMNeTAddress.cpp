/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTAddress.h"
#include <hash_fun.h>

using namespace std;

OMNETARA_NAMESPACE_BEGIN

std::string OMNeTAddress::toString() const {
    char buffer[6];
    char* string = buffer;
    sprintf(string, "[%u]", getAddressByte(5));
    *(string-1) = '\0';
    return std::string(buffer);
}

bool OMNeTAddress::equals(const Address* otherAddress) const {
    const OMNeTAddress* otherOMNeTAddress = dynamic_cast<const OMNeTAddress*>(otherAddress);
    if(otherOMNeTAddress == NULL) {
        return false;
    }
    else {
        // use the native implementation of IPv4Address
        return MACAddress::equals(*otherOMNeTAddress);
    }
}

bool OMNeTAddress::equals(const AddressPtr otherAddress) const {
    return this->equals(otherAddress.get());
}

size_t OMNeTAddress::getHashValue() const {
    return getInt() % sizeof(size_t);
}

Address* OMNeTAddress::clone() {
    OMNeTAddress* clone = new OMNeTAddress(getInt());
    return clone;
}

OMNETARA_NAMESPACE_END
