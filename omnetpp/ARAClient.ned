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

package ara;

import inet.base.NotificationBoard;
import ara.ARANetworkLayer;
//import ara.OMNeTBattery;
import inet.battery.InetSimpleBattery;
import inet.linklayer.ieee80211.Ieee80211NicAdhoc;
import inet.mobility.NullMobility;
import inet.nodes.wireless.WirelessHost;
import inet.applications.pingapp.PingApp;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;

module ARAClient
{
    parameters:
        @display("i=device/wifilaptop;bgb=366,335");
        int nrOfNetworkInterfaces = default(1);
    gates:
        input radioIn @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=50,94");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=50,163");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "";
                @display("p=50,228;i=block/table");
        }
        app: PingApp {
            parameters:
                @display("b=40,24;p=239,29,col");
        }
        ara: ARANetworkLayer {
            parameters:
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=239,124");
        }
        wlan[nrOfNetworkInterfaces]: Ieee80211NicAdhoc {
            parameters:
                @display("p=239,222;q=queue");
        }

        // Stationary node
        mobility: NullMobility {
            parameters:
                @display("p=50,287");
        }

//        battery : OMNeTBattery {
        battery : InetSimpleBattery {
	        parameters:
		        @display("p=65,310;i=block/plug");
        }

    connections allowunconnected:
        app.pingOut --> ara.upperLayerGate$i;
        app.pingIn <-- ara.upperLayerGate$o;

        for i=0..nrOfNetworkInterfaces-1 {
            ara.ifOut[i] --> wlan[i].uppergateIn;
            ara.ifIn[i] <-- wlan[i].uppergateOut;
            wlan[i].radioIn <-- radioIn;
        }

}
