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

#ifndef ARACLIENTMOCK_H_
#define ARACLIENTMOCK_H_

#include "AbstractARAClient.h"
#include "ForwardingPolicy.h"
#include "Packet.h"
#include "NetworkInterfaceMock.h"
#include "PacketTrap.h"
#include "RoutingTable.h"

#include <string>
#include <deque>

namespace ARA {

/**
 * A ARAClientMock implements the abstract ARA Client and holds references to
 * other mocks for testing the routing table, packet trap and network interfaces.
 */
class ARAClientMock: public AbstractARAClient {
public:
    ARAClientMock();
    ~ARAClientMock();

    ForwardingPolicy* getForwardingPolicy();
    void updateRoutingTable(const Packet* packet, NetworkInterface* interface);
    void deliverToSystem(const Packet* packet);

    PacketTrap* getPacketTrap();
    RoutingTable* getRoutingTable();
    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "DEFAULT");
    std::deque<const Packet*>* getDeliveredPackets();
    void setEvaporationPolicy(EvaporationPolicy *policy);

private:
    std::deque<NetworkInterfaceMock*> interfaceMocks;
    std::deque<const Packet*> deliveredPackets;

    ForwardingPolicy* forwardingPolicy;
    EvaporationPolicy* evaporationPolicy;
};

} /* namespace ARA */
#endif /* ARACLIENTMOCK_H_ */
