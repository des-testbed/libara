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
@namespace(ARA);
package ara_simulation.omnetpp;

simple OMNeTARAClient
{
    parameters:
        string forwardingPolicy = default("stochastic");
        double deltaPhi = default(1);
        int nrOfNetworkInterfaces = default(1);

    gates:
        inout  upperLayerGate;
        input  ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)
        input arpIn;                            // input from ARP
        output arpOut;                          // output to ARP
}