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

package ara;

import ara.EARA;

//import ara.OMNeTEnergyAwareRoutingTable; 
import ara.reinforcement.OMNeTLinearPathReinforcementPolicy;
import ara.evaporation.OMNeTExponentialEvaporationPolicy;
import ara.forwarding.OMNeTEnergyAwareStochasticForwardingPolicy;
import inet.networklayer.arp.ARP;

module EARANetworkLayer 
{
    parameters:
        int nrOfNetworkInterfaces = default(1);

        @display("i=ara.png;bgb=301,297");

    gates:
        inout upperLayerGate;
        input ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)

    submodules:
        eara: EARA {
            parameters:
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=204,94");
        }

        araRoutingTable: OMNeTRoutingTable {
            parameters:
                @display("p=72,40;i=block/table");
        }

        forwardingPolicy: OMNeTEnergyAwareStochasticForwardingPolicy {
            parameters:
                @display("p=72,111");
        }

        reinforcementPolicy: OMNeTLinearPathReinforcementPolicy {
            parameters:
                @display("p=72,185");
        }

        evaporationPolicy: OMNeTExponentialEvaporationPolicy {
            parameters:
                @display("p=72,254");
        }

        arp: ARP {
            parameters:
                proxyARP = false;
                @display("p=204,215;q=pendingQueue");
            gates:
                nicOut[nrOfNetworkInterfaces];
        }

    connections allowunconnected:
        upperLayerGate <--> eara.upperLayerGate;
        arp.ipIn <-- eara.arpOut;

        for i=0..nrOfNetworkInterfaces-1 {
            ifIn[i] --> eara.ifIn[i];
            arp.nicOut[i] --> ifOut[i];
        }
}
