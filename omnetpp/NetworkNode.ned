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
import ara.OMNeTBattery;
import inet.linklayer.ieee80211.Ieee80211Nic;
import inet.mobility.models.RandomWPMobility;
import inet.mobility.IMobility;
import inet.nodes.wireless.WirelessHost;
import inet.applications.pingapp.PingApp;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;

module NetworkNode
{
    parameters:
        @node;
        @display("i=device/wifilaptop;bgb=366,335");
        int nrOfNetworkInterfaces = default(1);
        string mobilityType = default("RandomWPMobility");

    gates:
        input radioIn @directIn;

    submodules:
        notificationBoard: NotificationBoard {
            parameters:
                @display("p=49,37");
        }
        interfaceTable: InterfaceTable {
            parameters:
                @display("p=49,106");
        }
        routingTable: RoutingTable {
            parameters:
                IPForward = true;
                routerId = "";
                @display("p=49,171;i=block/table");
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
        wlan[nrOfNetworkInterfaces]: Ieee80211Nic {
            parameters:
                // there is now only one nic which is controlled in its behavior by various parameters
                mgmtType = "Ieee80211MgmtAdhoc";
                @display("p=239,222;q=queue");
        }

        // Stationary node
        mobility: <mobilityType> like IMobility {
            parameters:
                @display("p=49,230");
        }

        battery: OMNeTBattery {
            parameters:
                @display("p=49,293;i=block/plug");
        }

    connections allowunconnected:
        app.pingOut --> ara.upperLayerGate$i;
        app.pingIn <-- ara.upperLayerGate$o;

        for i=0..nrOfNetworkInterfaces-1 {
            ara.ifOut[i] --> wlan[i].upperLayerIn;
            ara.ifIn[i] <-- wlan[i].upperLayerOut;
            wlan[i].radioIn <-- radioIn;
        }

}
