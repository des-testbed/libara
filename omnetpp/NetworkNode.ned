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
import ara.traffic.TrafficGenerator;
import ara.traffic.TrafficSink;
import ara.traffic.TrafficMultiplexer;
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

module NetworkNode
{
    parameters:
        @node;
        @display("i=device/wifilaptop;bgb=366,335");

        string posX = default("auto"); // the position parameter can be used to assign a position to a node even if it is part of a node vector. Allowed values are "auto", "center", "left", "right", "top", "bottom" or any postive integer
        string posY = default("auto"); // see posX
        int nrOfNetworkInterfaces = default(1);
        int nrOfTrafficGenerators = default(1);

        string mobilityType = default("RandomWPMobility");
        string batteryModel = default("OMNeTBattery");

        string forwardingPolicyModel = default("OMNeTStochasticForwardingPolicy");
        string evaporationModel = default("OMNeTExponentialEvaporationPolicy");
        string reinforcementModel = default("OMNeTLinearPathReinforcementPolicy");

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

        app[nrOfTrafficGenerators]: TrafficGenerator {
            parameters:
                @display("b=40,24;p=205,29,col");
        }

        sink: TrafficSink {
            parameters:
                @display("b=40,24;p=324,29,col");
        }

        mux: TrafficMultiplexer {
			parameters:
			    nrOfUpperGates = nrOfTrafficGenerators;
            	@display("p=205,100");
        }

        networkLayer: ARANetworkLayer {
            parameters:
                evaporationModel = evaporationModel;
                forwardingPolicyModel = forwardingPolicyModel;
                reinforcementModel = reinforcementModel;
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=239,185");
        }

        wlan[nrOfNetworkInterfaces]: Ieee80211Nic {
            parameters:
                mgmtType = "Ieee80211MgmtAdhoc";
                @display("p=239,268;q=queue");
        }

        mobility: <mobilityType> like IMobility if mobilityType != "" {
            parameters:
                @display("p=49,230");
        }

    connections allowunconnected:
        for i=0..nrOfTrafficGenerators-1 {
            app[i].toLowerGate --> mux.fromUpperGate[i];
        }
        mux.toLowerGate --> networkLayer.upperLayerGate$i;
        sink.fromLowerGate <-- networkLayer.upperLayerGate$o;

        for i=0..nrOfNetworkInterfaces-1 {
            networkLayer.ifOut[i] --> wlan[i].upperLayerIn;
            networkLayer.ifIn[i] <-- wlan[i].upperLayerOut;
            wlan[i].radioIn <-- radioIn;
        }
}
