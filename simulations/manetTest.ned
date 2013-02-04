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
import ara_simulation.omnetpp.Debugger;
import ara_simulation.omnetpp.OMNeTARAClient;
import inet.linklayer.ieee80211.Ieee80211NicAdhoc;
import inet.mobility.NullMobility;
import inet.nodes.wireless.WirelessHost;
import inet.applications.pingapp.PingApp;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.networklayer.arp.ARP;

import inet.world.ChannelControl;

module ARAClient
{
    parameters:
        @display("i=device/wifilaptop;bgb=344,315");
        int nrOfNetworkInterfaces = default(1);
    gates:
        input radioIn @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=50,31");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=50,100");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "";
                @display("p=50,165;i=block/table");
        }
        app: PingApp {
            parameters:
                @display("b=40,24;p=178,31,col");
        }
        ara: OMNeTARAClient {
            parameters:
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("p=178,126");
        }
        arp: ARP {
            parameters:
                proxyARP = true;
                @display("p=305,216;q=pendingQueue");
            gates:
                nicOut[nrOfNetworkInterfaces];
        }
        wlan[nrOfNetworkInterfaces]: Ieee80211NicAdhoc {
            parameters:
                @display("p=178,245;q=queue");
        }

        // Stationary node
        mobility: NullMobility {
            parameters:
                @display("p=50,224");
        }
    connections allowunconnected:
        app.pingOut --> ara.upperLayerGate$i;
        app.pingIn <-- ara.upperLayerGate$o;

        arp.ipIn <-- ara.arpOut;

        for i=0..nrOfNetworkInterfaces-1 {
            ara.ifOut[i] --> wlan[i].uppergateIn;
            wlan[i].uppergateOut --> ara.ifIn[i];
            wlan[i].radioIn <-- radioIn;
        }

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

        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=61,46");
        }
}

