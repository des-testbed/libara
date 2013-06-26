/*
 * $FU-Copyright$
 */

#include "TestbedAddress.h" 

TESTBED_NAMESPACE_BEGIN

TestbedAddress::TestbedAddress(u_char* address){
   memcpy(this->address, address, 6);
}

std::string TestbedAddress::toString() const{
    char buffer[12];
    snprintf(buffer, ETHER_ADDR_LEN+6, "%X:%X:%X:%X:%X:%X", address[0], address[1], address[2], address[3], address[4], address[5]);
    return buffer;
}

bool TestbedAddress::equals(const Address* otherAddress) const{
    const TestbedAddress* otherTestbedAddress = dynamic_cast<const TestbedAddress*>(otherAddress);
    if(otherTestbedAddress == NULL) {
        return false;
    }

    for(int i=0; i<ETHER_ADDR_LEN; i++) {
        if(this->address[i] != otherTestbedAddress->address[i]) {
            return false;
        }
    }
   return true;
}

bool TestbedAddress::equals(const std::shared_ptr<Address> otherAddress) const {
    return this->equals(otherAddress.get());
}

size_t TestbedAddress::getHashValue() const {
    int leastSignificantByte = address[5];
    int nextLeastSignificantByte = address[4];
    return leastSignificantByte * 256 + nextLeastSignificantByte;
}

TESTBED_NAMESPACE_END
