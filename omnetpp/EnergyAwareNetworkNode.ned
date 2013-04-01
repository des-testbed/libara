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

import ara.EARANetworkLayer;
import ara.OMNeTBattery;
import inet.applications.pingapp.PingApp;
import inet.base.NotificationBoard;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.linklayer.ieee80211.Ieee80211Nic;
import inet.mobility.IMobility;

module EnergyAwareNetworkNode {
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
        
        networkLayer: EARANetworkLayer {
            parameters:
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=239,124");
        }
        
        wlan[nrOfNetworkInterfaces]: Ieee80211Nic {
            parameters:
                mgmtType = "Ieee80211MgmtAdhoc";
                @display("p=239,222;q=queue");
        }

        mobility: <mobilityType> like IMobility {
            parameters:
                @display("p=49,230");
        }
        
        battery: OMNeTBattery {
            //        battery : InetSimpleBattery {
            parameters:
                @display("p=49,293;i=block/plug");
        }

    connections allowunconnected:
        app.pingOut --> networkLayer.upperLayerGate$i;
        app.pingIn <-- networkLayer.upperLayerGate$o;

        for i=0..nrOfNetworkInterfaces-1 {
            networkLayer.ifOut[i] --> wlan[i].upperLayerIn;
            networkLayer.ifIn[i] <-- wlan[i].upperLayerOut;
            wlan[i].radioIn <-- radioIn;
        }
}
