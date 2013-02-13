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
#include "IPAddressResolver.h"
#include "IPControlInfo.h"

using namespace std;

namespace ARA {
namespace omnetpp {

OMNeTGate::OMNeTGate(cSimpleModule* module, cGate* gate) {
    this->module = module;
    this->gate = gate;

    cModule *host = module->getParentModule();

    // find IInterfaceTable
    cModule *mod = host->getSubmodule("interfaceTable");
    if (!mod)
        opp_error("IPAddressResolver: IInterfaceTable not found as submodule "
                  " `interfaceTable' in host/router `%s'", host->getFullPath().c_str());
    IInterfaceTable* interfaceTable = check_and_cast<IInterfaceTable *>(mod);

    InterfaceEntry* interfaceEntry = interfaceTable->getInterfaceByNodeInputGateId(gate->getNextGate()->getId());

    IPvXAddress localAddress = IPAddressResolver().getAddressFrom(interfaceEntry, IPAddressResolver::ADDR_IPv4);
    this->localAddress = shared_ptr<Address>(new OMNeTAddress(localAddress.get4()));
}

void OMNeTGate::send(const Packet* packet, shared_ptr<Address> recipient) {
    OMNeTPacket* omnetPacket = (OMNeTPacket*) packet->clone();

    IPControlInfo* ctrl = new IPControlInfo();
    ctrl->setSrcAddr(omnetPacket->getSource()->getAddress());
    ctrl->setDestAddr(omnetPacket->getDestination()->getAddress());
    omnetPacket->setControlInfo(ctrl);

    module->send(omnetPacket, gate);
}

void OMNeTGate::broadcast(const Packet* packet) {
    EV << "Broadcasting something\n";
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

bool OMNeTGate::isBroadcastAddress(std::shared_ptr<Address> someAddress) const {
//    IPAddress someIPAddress = IPAddress(192, 168, 0, 1);
//    IPAddress subnetMask = IPAddress(255, 240, 0, 0);
//    IPAddress networkAddress = someIPAddress.doAnd(subnetMask);
//    IPAddress broadcastAddress = networkAddress.getBroadcastAddress(subnetMask);
//
//    return someAddress
    return false; //FIXE echte Adresse berechnen
}

} /* namespace omnetpp */
} /* namespace ARA */
