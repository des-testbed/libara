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

package ara.simulations.basic;

import ara.NetworkNode;
import ara.ARANetworkConfigurator;
import ara.time.OMNeTClock;
import inet.world.radio.ChannelControl;

network Scenario04
{
    parameters:
        @display("b=297,203;bgb=1059,579");

    submodules:
        clock: OMNeTClock {
            @display("p=72,170");
        }

        channelControl: ChannelControl {
            parameters:
                @display("p=72,36");
        }

        networkConfigurator: ARANetworkConfigurator {
            parameters:
                networkAddress = "192.168.0.0";
                netmask = "255.255.0.0";
                @display("p=72,103");
        }

        node1: NetworkNode {
            @display("p=198,288");
        }

        node2: NetworkNode {
            @display("p=370,206");
        }

        node3: NetworkNode {
            @display("p=370,413");
        }

        node4: NetworkNode {
            @display("p=535,356");
        }

        node5: NetworkNode {
            @display("p=528,65");
        }

        node6: NetworkNode {
            @display("p=717,470");
        }

        node7: NetworkNode {
            @display("p=694,190");
        }

        node8: NetworkNode {
            @display("p=860,302");
        }
}

