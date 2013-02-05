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

import ara.omnetpp.ARAClient;
import inet.world.ChannelControl;

network ManetTest {
    parameters:
        int numberOfHosts;
        double playgroundSizeX;
        double playgroundSizeY;
        @display("b=297,203");
    submodules:
        node[numberOfHosts]: ARAClient {
            parameters:
                @display("r=,,#707070");
        }        

        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=61,46");
        }
}

