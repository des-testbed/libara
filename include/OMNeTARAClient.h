//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef OMNETARACLIENT_H_
#define OMNETARACLIENT_H_

#include <csimplemodule.h>
#include "AbstractARAClient.h"
#include "NetworkInterface.h"
#include "NextHop.h"
#include "Packet.h"

namespace ARA {

class OMNeTARAClient: public cSimpleModule, public AbstractARAClient {

protected:
    //~~~ INHERITED FROM cSimpleModule ~~~~~~~
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    //~~~ INHERITED FROM AbstractARAClient ~~~
    NextHop* getNextHop(const Packet* packet);
    void updateRoutingTable(const Packet* packet, NetworkInterface* interface);
    void deliverToSystem(const Packet* packet);
};

} /* namespace ARA */
#endif /* OMNETARACLIENT_H_ */
