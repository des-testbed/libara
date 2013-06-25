#include "TestbedAddress.h" 

TESTBED_NAMESPACE_BEGIN

TestbedAddress::TestbedAddress(int byte1, int byte2, int byte3, int byte4, int byte5, int byte6){
   if(snprintf((char*)this->address, ETHER_ADDR_LEN, "%X:%X:%X:%X:%X:%X", byte1, byte2, byte3, byte4, byte5, byte6) < 0){
       dessert_debug("TestbedAddress initialization failure for address %X:%X:%X:%X:%X:%X", byte1, byte2, byte3, byte4, byte5, byte6);
   }
}

std::string TestbedAddress::toString() const{
    return std::string((const char*)address);
}

bool TestbedAddress::equals(const Address* otherAddress) const{
    const TestbedAddress* otherTestbedAddress = dynamic_cast<const TestbedAddress*>(otherAddress);
    if(otherTestbedAddress == NULL) {
        return false;
    }

    for(int i=0; i<ETHER_ADDR_LEN; i++)
    {
        if(this->address[i] != otherTestbedAddress->address[i])
        {
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

Address* TestbedAddress::clone() {
   return new TestbedAddress(address[0],address[1],address[2],address[3],address[4],address[5]);
}

TESTBED_NAMESPACE_END
