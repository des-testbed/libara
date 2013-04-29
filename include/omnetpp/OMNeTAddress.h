/*
 * $FU-Copyright$
 */

#ifndef OMNETADDRESS_H_
#define OMNETADDRESS_H_

#include "OMNeTARAMacros.h"
#include "Address.h"
#include "MACAddress.h"

#include <string>

OMNETARA_NAMESPACE_BEGIN

class OMNeTAddress : public Address, public MACAddress {
public:

    OMNeTAddress(uint64 bits) : MACAddress(bits) {}
    OMNeTAddress(const MACAddress& someMACAddress) : MACAddress(someMACAddress) {}

    std::string toString() const;
    bool equals(const Address* otherAddress) const;
    bool equals(const std::shared_ptr<Address> otherAddress) const;
    bool equals(const MACAddress& ipAddress) const {return MACAddress::equals(ipAddress);}

    size_t getHashValue() const;
    Address* clone();
};

OMNETARA_NAMESPACE_END

#endif
