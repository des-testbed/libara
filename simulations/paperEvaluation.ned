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

package ara.simulations;

import ara.NetworkNode;
import ara.ARANetworkConfigurator;
import ara.time.OMNeTClock;
import inet.world.ChannelControl;

network PaperEvaluation
{
    parameters:
        double playgroundSizeX;
        double playgroundSizeY;
        @display("b=297,203;bgb=758,337");

    submodules:
        clock: OMNeTClock {
            @display("p=78,161");
        }
        
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=78,36");
        }

        networkConfigurator: ARANetworkConfigurator {
            parameters:
                networkAddress = "192.168.0.0";
                netmask = "255.255.0.0";
                @display("p=78,97");
        }

        node[50] : NetworkNode;
}

