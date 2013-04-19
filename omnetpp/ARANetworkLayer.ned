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

import ara.ARA;
import ara.reinforcement.OMNeTLinearPathReinforcementPolicy;

import ara.evaporation.EvaporationPolicy;
import ara.evaporation.OMNeTCubicEvaporationPolicy;
import ara.evaporation.OMNeTLinearEvaporationPolicy;
import ara.evaporation.OMNeTExponentialEvaporationPolicy;

import ara.forwarding.ForwardingPolicy;
import ara.forwarding.OMNeTStochasticForwardingPolicy;
import ara.forwarding.OMNeTBestPheromoneForwardingPolicy;
import ara.forwarding.OMNeTEnergyAwareStochasticForwardingPolicy;

import inet.networklayer.arp.ARP;

module ARANetworkLayer
{
    parameters:
        int nrOfNetworkInterfaces = default(1);
        string forwardingPolicyModel @enum("OMNeTStochasticForwardingPolicy","OMNeTEnergyAwareStochasticForwardingPolicy","OMNeTBestPheromoneForwardingPolicy") = default("OMNeTStochasticForwardingPolicy");
        string evaporationModel @enum("OMNeTExponentialEvaporationPolicy","OMNeTCubicEvaporationPolicy","OMNeTLinearEvaporationPolicy") = default("OMNeTExponentialEvaporationPolicy");

        @display("i=ara.png;bgb=301,241");

    gates:
        inout upperLayerGate;
        input ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)

    submodules:
        ara: ARA {
            parameters:
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=204,72");
        }

        forwardingPolicy: <forwardingPolicyModel> like ForwardingPolicy if forwardingPolicyModel != "" {
            parameters:
                @display("p=71,33");
        }

        reinforcementPolicy: OMNeTLinearPathReinforcementPolicy {
            parameters:
                @display("p=71,107");
        }

        evaporationPolicy: <evaporationModel> like EvaporationPolicy if evaporationModel != "" {
            parameters:
                @display("p=71,176");
        }

        arp: ARP {
            parameters:
                proxyARP = false;
                @display("p=204,160;q=pendingQueue");
            gates:
                nicOut[nrOfNetworkInterfaces];
        }


    connections allowunconnected:
        upperLayerGate <--> ara.upperLayerGate;
        arp.ipIn <-- ara.arpOut;

        for i=0..nrOfNetworkInterfaces-1 {
            ifIn[i] --> ara.ifIn[i];
            arp.nicOut[i] --> ifOut[i];
        }
}
