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
@namespace(ARA::omnetpp);
package ara;


simple ARATrafficGenerator
{
    parameters:
        string destAddresses; // list of IP addresses, separated by spaces
        int numberOfPackets = default(50);
        double packetInterval @unit("s") = default(10ms);
        int packetLength @unit("B");
        int protocol = default(4); // value for IPDatagram protocol field
        int defaultBroadCastAddressInterface=default(101);
        @display("i=block/app");
    gates:
        input in;
        output out;
}
