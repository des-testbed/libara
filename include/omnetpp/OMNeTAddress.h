/*
 * $FU-Copyright$
 */

#ifndef OMNETADDRESS_H_
#define OMNETADDRESS_H_

#include "Address.h"
#include "IPAddress.h"

#include <string>

namespace ARA {
namespace omnetpp {

class OMNeTAddress : public Address, public IPAddress {
public:

    /**
     * IP address as int
     */
    OMNeTAddress(uint32 ipAddress) : IPAddress(ipAddress) {}

    /**
     * IP address bytes: "byte1.byte2.byte3.byte4" format
     */
    OMNeTAddress(int byte1, int byte2, int byte3, int byte4) : IPAddress(byte1, byte2, byte3, byte4) {}

    /**
     * IP address given as text: "192.66.86.1"
     */
    OMNeTAddress(const char* ipAsString) : IPAddress(ipAsString) {}

    /**
     * Copy from some IPAddress
     */
    OMNeTAddress(const IPAddress& someIPAddress) : IPAddress(someIPAddress) {}

    const char* toString() const;
    bool equals(const Address* otherAddress) const;
    bool equals(const std::shared_ptr<Address> otherAddress) const;
    bool equals(const IPAddress& ipAddress) const {return IPAddress::equals(ipAddress);}

    size_t getHashValue() const;
    Address* clone();
};

} /* namespace omnetpp */
} /* namespace ARA */
#endif /* OMNETADDRESS_H_ */
