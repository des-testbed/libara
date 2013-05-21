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

simple ARANetworkConfigurator { 
	parameters:
        string networkAddress = default("192.168.0.0"); // network part of the address (see netmask parameter)
        string netmask = default("255.255.0.0"); // host part of addresses are autoconfigured
        bool persistStartPositions = default(false); // wether to save the starting position of nodes to a file in the results folder called <EXPERIMENT_NAME>-StartPositions.txt
        @display("i=block/cogwheel_s");
        @labels(node);
}
