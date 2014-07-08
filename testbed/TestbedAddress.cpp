/*
 * $FU-Copyright$
 */

#include "TestbedAddress.h" 

TESTBED_NAMESPACE_BEGIN

TestbedAddress::TestbedAddress(u_char* address){
    std::copy(address, address + 6, this->address);
}

std::string TestbedAddress::toString() const {
    std::lock_guard<std::mutex> lock(addressMutex);
    std::ostringstream result;
    /// set output to hex values
    result << std::hex;
    /// insert colons between hex values
    std::copy(this->address, this->address+6, std::ostream_iterator<short>(result, ":"));
    /// the 'substr' workaround does not work if there are hex values with one digit in the address (so we have to determine the size)
    short length = result.str().size() - 1;
    /// since we don't want to write an infix operator and get rid of the last colon we return a substring
    return result.str().substr(0, length);
}

bool TestbedAddress::equals(const Address* otherAddress) const{
    //std::lock_guard<std::mutex> lock(addressMutex);
    const TestbedAddress* otherTestbedAddress = dynamic_cast<const TestbedAddress*>(otherAddress);

    if (otherTestbedAddress == nullptr) {
        return false;
    }

    std::string address = this->toString();
    return (address.compare(otherAddress->toString()) == 0);
}

bool TestbedAddress::equals(const std::shared_ptr<Address> otherAddress) const {
    //std::lock_guard<std::mutex> lock(addressMutex);
    return this->equals(otherAddress.get());
}

size_t TestbedAddress::getHashValue() const {
    //std::lock_guard<std::mutex> lock(addressMutex);
    int leastSignificantByte = address[5];
    int nextLeastSignificantByte = address[4];
    return leastSignificantByte * 256 + nextLeastSignificantByte;
}

u_int8_t* TestbedAddress::getDessertValue() const {
    // FIXME: we pass the pointer to something else ... (not good!)
    //std::lock_guard<std::mutex> lock(addressMutex);
    //TODO: Find a better way to return acceptable value for cross platform
    return (u_int8_t*) this->address;
}

TESTBED_NAMESPACE_END
