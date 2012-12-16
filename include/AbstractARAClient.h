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

#ifndef ABSTRACTARACLIENT_H_
#define ABSTRACTARACLIENT_H_

#include "NextHop.h"
#include "NetworkInterface.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "LinkedList.h"

namespace ARA {

class AbstractARAClient {
public:
    AbstractARAClient();
    virtual ~AbstractARAClient();

    virtual NextHop* getNextHop(Packet* packet) = 0;

    void addNetworkInterface(NetworkInterface* newInterface);
    NetworkInterface* getNetworkInterface(unsigned int index);
    unsigned int getNumberOfNetworkInterfaces();

    void sendPacket(Packet* packet);
    //TODO AbstractARAClient::broadCast(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    void broadCast(Packet* packet);
    //TODO AbstractARAClient::getNextSequenceNumber(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    unsigned int getNextSequenceNumber();

protected:

    LinkedList<NetworkInterface> interfaces;
    PacketTrap packetTrap;
    RoutingTable routingTable;

private:
    unsigned int nextSequenceNumber = 1;
};

} /* namespace ARA */
#endif /* ABSTRACTARACLIENT_H_ */
