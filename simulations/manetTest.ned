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

package ara_simulation.simulations;

// Imports for the ARAClient
import inet.base.NotificationBoard;
import ara_simulation.omnetpp.ARATrafficGenerator;
import inet.linklayer.ieee80211.Ieee80211NicSTASimplified;
import inet.mobility.NullMobility;

import inet.world.ChannelControl;

module ARAClient
{
    parameters:
        @display("i=device/wifilaptop");
    gates:
        input radioIn @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=52,70");
        }
        app: ARATrafficGenerator {
            parameters:
                @display("b=40,24;p=180,60,col");
        }

//        wlan: Ieee80211NicSTASimplified {
//            parameters:
//                @display("p=112,134;q=queue");
//        }
        
        // Stationary node
        mobility: NullMobility {
            parameters:
                @display("p=50,141");
        }
    connections allowunconnected:
//        wlan.radioIn <-- radioIn;
//        app.out --> wlan.uppergateIn;
}

network ManetTest
{
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
//        srvHost: ThroughputServer {
//            parameters:
//                @display("p=350,350;r=,,#707070");
//        }
//        ap: WirelessAPSimplified {
//            parameters:
//                @display("p=200,200;r=,,#707070");
//        }
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=61,46");
        }
}

