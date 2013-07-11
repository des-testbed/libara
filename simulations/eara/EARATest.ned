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

package ara.simulations.eara;

import ara.EnergyAwareNetworkNode;
import ara.ARANetworkConfigurator;
import ara.time.OMNeTClock;
import inet.world.radio.ChannelControl;

network EARATest
{
    parameters:
        @display("b=297,203;bgb=758,337");
        volatile double nodeSpeed @unit(mps) = default(0mps);
        volatile double nodePauseTime @unit(s) = default(1000s);

        string mobilityType = default("StaticMobility");
        volatile double mobilityUpdateIntervall @unit(ms) = default(100ms);

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

        node1: EnergyAwareNetworkNode {
            parameters:
                mobilityType = mobilityType;
                mobility.speed = nodeSpeed;
                mobility.waitTime = nodePauseTime;
                mobility.updateInterval = mobilityUpdateIntervall;
                @display("p=86,181");
        }
        node2: EnergyAwareNetworkNode {
            parameters:
                mobilityType = mobilityType;
                mobility.speed = nodeSpeed;
                mobility.waitTime = nodePauseTime;
                mobility.updateInterval = mobilityUpdateIntervall;
                @display("p=287,181");
        }
        node3: EnergyAwareNetworkNode {
            parameters:
                mobilityType = mobilityType;
                mobility.speed = nodeSpeed;
                mobility.waitTime = nodePauseTime;
                mobility.updateInterval = mobilityUpdateIntervall;
                @display("p=483,181");
        }
}

