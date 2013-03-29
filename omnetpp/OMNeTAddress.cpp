/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTAddress.h"
#include <hash_fun.h>

using namespace std;

namespace ARA {
namespace omnetpp {

const char* OMNeTAddress::toString() const {
    return this->str().c_str();
}

bool OMNeTAddress::equals(const Address* otherAddress) const {
    const OMNeTAddress* otherOMNeTAddress = dynamic_cast<const OMNeTAddress*>(otherAddress);
    if(otherOMNeTAddress == NULL) {
        return false;
    }
    else {
        // use the native implementation of IPv4Address
        return IPv4Address::equals(*otherOMNeTAddress);
    }
}

bool OMNeTAddress::equals(const std::shared_ptr<Address> otherAddress) const {
    return this->equals(otherAddress.get());
}

size_t OMNeTAddress::getHashValue() const {
    int firstByte = getDByte(0);
    int secondByte = getDByte(1);
    return firstByte * 256 + secondByte;  // integer value between 0 and 65535
}

Address* OMNeTAddress::clone() {
    OMNeTAddress* clone = new OMNeTAddress(getInt());
    return clone;
}

} /* namespace omnetpp */
} /* namespace ARA */
