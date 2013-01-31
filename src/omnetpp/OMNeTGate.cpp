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
#include "IPAddressResolver.h"

using namespace std;

namespace ARA {

OMNeTGate::OMNeTGate(cSimpleModule* module, cGate* gate) {
    this->module = module;
    this->gate = gate;
    cModule* connectedModule = gate->getNextGate()->getOwnerModule();
    IPvXAddress localAddress = IPAddressResolver().addressOf(module->getParentModule(), connectedModule->getFullName(), IPAddressResolver::ADDR_IPv4);
    this->localAddress = shared_ptr<Address>(new OMNeTAddress(localAddress.get4()));
}

void OMNeTGate::send(const Packet* packet, shared_ptr<Address> recipient) {
    OMNeTPacket* omnetPacket = (OMNeTPacket*) packet->clone();
    module->send(omnetPacket, gate);
}

void OMNeTGate::broadcast(const Packet* packet) {
    OMNeTPacket* omnetPacket = (OMNeTPacket*) packet->clone();
    module->send(omnetPacket, gate);
}

bool OMNeTGate::equals(NetworkInterface* otherInterface) {
    OMNeTGate* otherOMNeTInterface = dynamic_cast<OMNeTGate*>(otherInterface);
    if(otherOMNeTInterface == NULL) {
        return false;
    }
    else {
        return strcmp(module->getFullName(), otherOMNeTInterface->module->getFullName()) == 0
            && strcmp(gate->getFullName(), otherOMNeTInterface->gate->getFullName()) == 0;
    }
}

shared_ptr<Address> OMNeTGate::getLocalAddress() {
    return localAddress;
}

} /* namespace ARA */
