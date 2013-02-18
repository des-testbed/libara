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

#include "OMNeTGate.h"
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"
#include "IInterfaceTable.h"
#include "IPv4InterfaceData.h"
#include "IPAddressResolver.h"
#include "IPControlInfo.h"
#include "IPDatagram.h"

using namespace std;

namespace ARA {
namespace omnetpp {

typedef std::shared_ptr<OMNeTAddress> OMNeTAddressPtr;

OMNeTGate::OMNeTGate(cSimpleModule* module, cGate* gateToARP, InterfaceEntry* interfaceEntry) {
    this->module = module;
    this->gateToARP = gateToARP;

    IPAddress localAddress = IPAddressResolver().getAddressFrom(interfaceEntry, IPAddressResolver::ADDR_IPv4).get4();
    IPAddress netmask = interfaceEntry->ipv4Data()->getNetmask();
    IPAddress networkAddress = localAddress.doAnd(netmask);
    IPAddress broadcastAddress = networkAddress.getBroadcastAddress(netmask);

    this->localAddress = shared_ptr<Address>(new OMNeTAddress(localAddress));
    this->broadcastAddress = shared_ptr<Address>(new OMNeTAddress(broadcastAddress));
    this->interfaceID = interfaceEntry->getInterfaceId();
}

void OMNeTGate::send(const Packet* packet, shared_ptr<Address> recipient) {
    OMNeTPacket* omnetPacket = (OMNeTPacket*) packet->clone();
    OMNeTAddressPtr nextHopAddress = getNextHopAddress(recipient);

    IPRoutingDecision* controlInfo = new IPRoutingDecision();
    controlInfo->setNextHopAddr(*(nextHopAddress.get()));
    controlInfo->setInterfaceId(interfaceID);
    omnetPacket->setControlInfo(controlInfo);

    module->send(omnetPacket, gateToARP);
}

OMNeTAddressPtr OMNeTGate::getNextHopAddress(shared_ptr<Address> recipient) {
    shared_ptr<OMNeTAddress> nextHopAddress (dynamic_pointer_cast<OMNeTAddress>(recipient));
    if(nextHopAddress == NULL) {
        throw cRuntimeError("Error in OMNeTGate: Can only send packets to OMNeTAddress recipients");
    }
    return nextHopAddress;
}

void OMNeTGate::broadcast(const Packet* packet) {
    send(packet, broadcastAddress);
}

bool OMNeTGate::equals(NetworkInterface* otherInterface) {
    OMNeTGate* otherOMNeTInterface = dynamic_cast<OMNeTGate*>(otherInterface);
    if(otherOMNeTInterface == NULL) {
        return false;
    }
    else {
        return strcmp(module->getFullName(), otherOMNeTInterface->module->getFullName()) == 0
            && strcmp(gateToARP->getFullName(), otherOMNeTInterface->gateToARP->getFullName()) == 0;
    }
}

shared_ptr<Address> OMNeTGate::getLocalAddress() {
    return localAddress;
}

bool OMNeTGate::isBroadcastAddress(std::shared_ptr<Address> someAddress) const {
    return someAddress->equals(broadcastAddress);
}

} /* namespace omnetpp */
} /* namespace ARA */
