/*
 * $FU-Copyright$
 */

#include "TestbedAddress.h" 

TESTBED_NAMESPACE_BEGIN

TestbedAddress::TestbedAddress(u_char* address){
   memcpy(this->address, address, 6);
}

std::string TestbedAddress::toString() const{
    char buffer[18];
    snprintf(buffer, 18, "%x:%x:%x:%x:%x:%x", address[0], address[1], address[2], address[3], address[4], address[5]);
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


u_int8_t* TestbedAddress::getDessertValue() const {
    //TODO: Find a better way to return acceptable value for cross platform
    return (u_int8_t*) this->address;
}

TESTBED_NAMESPACE_END
