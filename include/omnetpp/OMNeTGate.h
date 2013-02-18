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

#ifndef OMNETGATE_H_
#define OMNETGATE_H_

#include "NetworkInterface.h"
#include "OMNeTAddress.h"
#include "InterfaceEntry.h"

#include <omnetpp.h>
#include <memory>

namespace ARA {
namespace omnetpp {

    class OMNeTGate: public ARA::NetworkInterface {
    public:
        OMNeTGate(cSimpleModule* module, cGate* gate, InterfaceEntry* interfaceEntry);

        void send(const Packet* packet, std::shared_ptr<Address> recipient);
        void broadcast(const Packet* packet);
        bool equals(NetworkInterface* interface);
        std::shared_ptr<Address> getLocalAddress();
        bool isBroadcastAddress(std::shared_ptr<Address> someAddress) const;

    private:
        std::shared_ptr<OMNeTAddress> getNextHopAddress(std::shared_ptr<Address> recipient);

    private:
        cSimpleModule* module;
        cGate* gateToARP;
        std::shared_ptr<Address> localAddress;
        std::shared_ptr<Address> broadcastAddress;
        int interfaceID;
    };

} /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNETGATE_H_ */
