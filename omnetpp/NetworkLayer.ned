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

import ara.ARA;
import ara.OMNetStochasticForwardingPolicy;
import ara.OMNeTLinearEvaporationPolicy;
import inet.networklayer.arp.ARP;

module NetworkLayer {
    parameters:
        int nrOfNetworkInterfaces = default(1);
        
        @display("i=ara.png;bgb=210,275");

    gates:
        inout upperLayerGate;
        input ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)

    submodules:
        ara: ARA {
            parameters:
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=100,78");
        }
        
        evaporationPolicy : OMNeTLinearEvaporationPolicy {
            
        }
        
        forwardingPolicy : OMNetStochasticForwardingPolicy {
        
        }

        arp: ARP {
            parameters:
                proxyARP = false;
                @display("p=100,191;q=pendingQueue");
            gates:
                nicOut[nrOfNetworkInterfaces];
        }

    connections allowunconnected:
        upperLayerGate <--> ara.upperLayerGate;
        arp.ipIn <-- ara.arpOut;
        
        for i = 0..nrOfNetworkInterfaces-1 {
            ifIn[i] --> ara.ifIn[i];
        }
}
