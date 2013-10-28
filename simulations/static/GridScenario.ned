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

package ara.simulations.static;

import ara.EnergyAwareNetworkNode;
import ara.ARANetworkConfigurator;
import ara.time.OMNeTClock;
import inet.world.radio.ChannelControl;

network GridScenario
{
    parameters:
        @display("b=297,203;bgb=758,337");
        int numberOfNodes;
        double playgroundSizeX @unit(m) = default(1000m);
        double playgroundSizeY @unit(m) = default(1000m);
        double playgroundSizeZ @unit(m) = default(0m);
        string mobilityType = default("StaticGridMobility");

    submodules:
        clock: OMNeTClock {
            @display("p=153,41;is=s");
        }

        channelControl: ChannelControl {
            parameters:
                @display("p=78,36");
        }

        networkConfigurator: ARANetworkConfigurator {
            parameters:
                networkAddress = "192.168.0.0";
                netmask = "255.255.0.0";
                @display("p=78,97");
        }

        node[numberOfNodes]: EnergyAwareNetworkNode {
            parameters:
                mobilityType = mobilityType;
                mobility.marginX = playgroundSizeX;
                mobility.marginY = playgroundSizeY;
                mobility.numHosts = numberOfNodes;
                mobility.seperation = 100.0m;
                mobility.constraintAreaMinX = 0m;
                mobility.constraintAreaMinY = 0m;
                mobility.constraintAreaMinZ = 0m;
                mobility.constraintAreaMaxX = playgroundSizeX;
                mobility.constraintAreaMaxY = playgroundSizeY;
                mobility.constraintAreaMaxZ = playgroundSizeZ;
        }
}

