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

#ifndef NETWORKINTERFACEMOCK_H_
#define NETWORKINTERFACEMOCK_H_

#include <memory>
#include "NetworkInterface.h"
#include "Packet.h"
#include "LinkedList.h"
#include "Pair.h"
#include "AddressMock.h"

namespace ARA {

class NetworkInterfaceMock: public ARA::NetworkInterface {
public:
    NetworkInterfaceMock();
    NetworkInterfaceMock(const std::string interfaceName);
    NetworkInterfaceMock(const std::string interfaceName, const std::string localAddressName);
    ~NetworkInterfaceMock();

    void send(const Packet* packet, Address* recipient);
    void broadcast(const Packet* packet);
    bool equals(NetworkInterface* otherInterface);
    std::shared_ptr<Address> getLocalAddress();

    std::string getName();
    LinkedList<Pair<Packet, Address>>* getSentPackets();
    unsigned int getNumberOfSentPackets();
    bool hasPacketBeenSend(Packet* packet);
    bool hasPacketBeenBroadCasted(Packet* packet);

private:
    std::string name;
    LinkedList<Pair<Packet, Address>> sentPackets;
    std::shared_ptr<Address> localAddress;
};

} /* namespace ARA */
#endif /* NETWORKINTERFACEMOCK_H_ */
