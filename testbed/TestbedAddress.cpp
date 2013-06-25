#include "TestbedAddress.h" 

TESTBED_NAMESPACE_BEGIN

TestbedAddress::TestbedAddress(int byte1, int byte2, int byte3, int byte4, int byte5, int byte6){
   if(snprintf((char*)this->address, ETHER_ADDR_LEN, "%d:%d:%d:%d:%d:%d", byte1, byte2, byte3, byte4, byte5, byte6) < 0){
       // an error occurred
   }
}


/// check why this should be null-terminated
std::string TestbedAddress::toString() const{
    return std::string((const char*)address);
}

bool TestbedAddress::equals(const Address* otherAddress) const{
   return false;
}

bool TestbedAddress::equals(const std::shared_ptr<Address> otherAddress) const {
   return false;
}

size_t TestbedAddress::getHashValue() const {
   return 42;
}

Address* TestbedAddress::clone() {
   return nullptr;
}

TESTBED_NAMESPACE_END
