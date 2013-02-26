/******************************************************************************
 Copyright 2012, The DES-ARA-SIM Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große, Michael Frey
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

#ifndef REINFORCEMENT_POLICY_H_
#define REINFORCEMENT_POLICY_H_

#include "RoutingTable.h"

namespace ARA {
    class ReinforcementPolicy {
         public:
             ReinforcementPolicy(){};
             ReinforcementPolicy(RoutingTable* pRoutingTable);
             ReinforcementPolicy(const ReinforcementPolicy& policy);
             virtual ~ReinforcementPolicy(){};

             virtual void update(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface) = 0;
             void setRoutingTable(RoutingTable* pRoutingTable);
             RoutingTable* getRoutingTable() const;

         private:
             RoutingTable* routingTable;
    };
}

#endif
