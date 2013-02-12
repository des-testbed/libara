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

#include <omnetpp.h>
#include <algorithm>
#include <csimplemodule.h>

#include "Packet.h"
#include "NextHop.h"
#include "OMNeTGate.h"
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"
#include "ForwardingPolicy.h"
#include "NetworkInterface.h"
#include "AbstractARAClient.h"
#include "BestPheromoneForwardingPolicy.h"
#include "OMNeTStochasticForwardingPolicy.h"

namespace ARA {
namespace omnetpp {

    /**
     * The class represents the implementation of the ant routing algorithm (ARA)
     * for the OMNeT++ simulation framework. 
     *
     * The algorithm was first published in:
     *
     *  Gunes, Mesut, Udo Sorges, and Imed Bouazizi. "ARA-the ant-colony based routing algorithm for MANETs." 
     *  Parallel Processing Workshops, 2002. Proceedings. International Conference on. IEEE, 2002.
     *
     */
    class OMNeTARAClient: public cSimpleModule, public ARA::AbstractARAClient {
        protected:
            //~~~ INHERITED FROM cSimpleModule ~~~~~~~
            virtual void initialize();
            virtual void handleMessage(cMessage *msg);

            //~~~ INHERITED FROM AbstractARAClient ~~~
            ForwardingPolicy* getForwardingPolicy();
            void updateRoutingTable(const Packet* packet, NetworkInterface* interface);
            void deliverToSystem(const Packet* packet);


        private:
            /// The member holds the forwarding policy, which defines how data packets are forwarded to the destination host
            ForwardingPolicy* forwardingPolicy;
            ///
            void sendInitialPacket();
            /// The method checks if the in NED file given policy exists and initializes the policy
            void initializeForwardingPolicy(std::string policy);
            /// The member denotes the constant which is used in the pheromone reinforcement of a path
            double deltaPhi;
    };

} /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNETARACLIENT_H_ */
