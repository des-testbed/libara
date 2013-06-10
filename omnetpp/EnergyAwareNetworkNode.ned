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

import ara.NetworkNodeBase;
import ara.EARANetworkLayer;
import ara.OMNeTBattery;

module EnergyAwareNetworkNode extends NetworkNodeBase
{
    parameters:
        @node;
        @display("i=device/wifilaptop;bgb=366,354");

    submodules:
        networkLayer: EARANetworkLayer {
            parameters:
                nrOfNetworkInterfaces = nrOfNetworkInterfaces;
                @display("i=ara;p=239,185");
        }

        battery: OMNeTBattery {
            parameters:
                @display("p=49,307;i=block/plug");
        }

    connections allowunconnected:
        mux.toLowerGate --> networkLayer.upperLayerGate$i;
        sink.fromLowerGate <-- networkLayer.upperLayerGate$o;

        for i=0..nrOfNetworkInterfaces-1 {
            networkLayer.ifOut[i] --> wlan[i].upperLayerIn;
            networkLayer.ifIn[i] <-- wlan[i].upperLayerOut;
        }
}
