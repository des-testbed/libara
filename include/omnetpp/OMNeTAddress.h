/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#ifndef OMNETADDRESS_H_
#define OMNETADDRESS_H_

#include "Address.h"
#include "IPAddress.h"

#include <string>

namespace ARA {
namespace omnetpp {

class OMNeTAddress : public ARA::Address, public IPAddress {
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

    bool equals(const Address* otherAddress) const;
    bool equals(const std::shared_ptr<Address> otherAddress) const;
    bool equals(const IPAddress& ipAddress) const {return IPAddress::equals(ipAddress);}

    size_t getHashValue() const;
    Address* clone();
};

} /* namespace omnetpp */
} /* namespace ARA */
#endif /* OMNETADDRESS_H_ */
