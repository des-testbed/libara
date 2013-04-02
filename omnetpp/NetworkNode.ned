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

import ara.ARANetworkLayer;
import ara.TrafficGenerator;
import inet.applications.pingapp.PingApp;
import inet.base.NotificationBoard;

import inet.transport.tcp.TCP;
import inet.transport.udp.UDP;
import inet.applications.IUDPApp;
import inet.applications.udpapp.UDPBasicBurst;
import inet.applications.udpapp.UDPSink;

import inet.networklayer.common.InterfaceTable;
import inet.networklayer.ipv4.RoutingTable;
import inet.linklayer.ieee80211.Ieee80211Nic;
import inet.mobility.IMobility;
import inet.battery.IBattery;

module NetworkNode {
    parameters:
        @node;
        @display("i=device/wifilaptop;bgb=366,335");

        int nrOfNetworkInterfaces = default(1);

        string mobilityType = default("RandomWPMobility");
        string batteryModel = default("OMNeTBattery");

        string forwardingPolicyModel = default("OMNeTStochasticForwardingPolicy");
        string evaporationModel = default("OMNeTExponentialEvaporationPolicy");

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

        app: TrafficGenerator {
            parameters:
                @display("b=40,24;p=239,29,col");
        }
        
        networkLayer: ARANetworkLayer {
            parameters:
                evaporationModel = evaporationModel;
                forwardingPolicyModel = forwardingPolicyModel;
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=239,124");
        }
        
        wlan[nrOfNetworkInterfaces]: Ieee80211Nic {
            parameters:
                mgmtType = "Ieee80211MgmtAdhoc";
                @display("p=239,222;q=queue");
        }

        mobility: <mobilityType> like IMobility if mobilityType != "" {
            parameters:
                @display("p=49,230");
        }

    connections allowunconnected:
        app.lowergate <--> networkLayer.upperLayerGate;

        for i=0..nrOfNetworkInterfaces-1 {
            networkLayer.ifOut[i] --> wlan[i].upperLayerIn;
            networkLayer.ifIn[i] <-- wlan[i].upperLayerOut;
            wlan[i].radioIn <-- radioIn;
        }
}
