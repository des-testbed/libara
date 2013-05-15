/*
 * $FU-Copyright$
 */

#ifndef OMNETADDRESS_H_
#define OMNETADDRESS_H_

#include "OMNeTARAMacros.h"
#include "Address.h"
#include "IPv4Address.h"

#include <string>

OMNETARA_NAMESPACE_BEGIN

class OMNeTAddress : public Address, public IPv4Address {
public:

    /**
      * IP address as int
      */
     OMNeTAddress(uint32 ipAddress) : IPv4Address(ipAddress) {}

     /**
      * IP address bytes: "byte1.byte2.byte3.byte4" format
      */
     OMNeTAddress(int byte1, int byte2, int byte3, int byte4) : IPv4Address(byte1, byte2, byte3, byte4) {}

     /**
      * IP address given as text: "192.66.86.1"
      */
     OMNeTAddress(const char* ipAsString) : IPv4Address(ipAsString) {}

     /**
      * Copy from some IPv4Address
      */
     OMNeTAddress(const IPv4Address& someIPv4Address) : IPv4Address(someIPv4Address) {}

    std::string toString() const;
    bool equals(const Address* otherAddress) const;
    bool equals(const std::shared_ptr<Address> otherAddress) const;
    bool equals(const IPv4Address& ipAddress) const {return IPv4Address::equals(ipAddress);}

    size_t getHashValue() const;
    Address* clone();
};

OMNETARA_NAMESPACE_END

#endif
