/*
 * $FU-Copyright$
 */

#include "TestbedAddress.h" 

TESTBED_NAMESPACE_BEGIN


TestbedAddress::TestbedAddress(u_char* address){
    std::copy(address, address + 6, this->address);
}

std::string TestbedAddress::toString() const {
    std::lock_guard<std::recursive_mutex> lock(addressMutex);

    std::ostringstream result;

    for (short i = 0; i < 5; i++) {
       result << std::setw(2) << std::setfill('0') << std::hex << (0xFF & address[i]) << ':';
    }
    result << std::setw(2) << std::setfill('0') << std::hex << (0xFF & address[5]);

    // TODO: check
    std::string resultString = result.str(); 

    return resultString;
}

bool TestbedAddress::equals(const Address* otherAddress) const{
    std::lock_guard<std::recursive_mutex> lock(addressMutex);
//    std::lock_guard<std::mutex> lock(addressMutex);
    const TestbedAddress* otherTestbedAddress = dynamic_cast<const TestbedAddress*>(otherAddress);

    if (otherTestbedAddress == nullptr) {
        return false;
    }

    std::string address = this->toString();
    return (address.compare(otherAddress->toString()) == 0);
}

bool TestbedAddress::equals(const std::shared_ptr<Address> otherAddress) const {
    std::lock_guard<std::recursive_mutex> lock(addressMutex);
    return this->equals(otherAddress.get());
}

size_t TestbedAddress::getHashValue() const {
//    std::lock_guard<std::mutex> lock(addressMutex);
    std::lock_guard<std::recursive_mutex> lock(addressMutex);
    int leastSignificantByte = address[5];
    int nextLeastSignificantByte = address[4];
    return leastSignificantByte * 256 + nextLeastSignificantByte;
}

u_int8_t* TestbedAddress::getDessertValue() const {
    std::lock_guard<std::recursive_mutex> lock(addressMutex);
    // FIXME: we pass the pointer to something else ... (not good!)
  //  std::lock_guard<std::mutex> lock(addressMutex);
    //TODO: Find a better way to return acceptable value for cross platform
    return (u_int8_t*) this->address;
}

TESTBED_NAMESPACE_END
