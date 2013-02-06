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
@namespace(ARA);
package ara.simulations;

simple OMNeTARAClient
{
    parameters:
        string policy;
        double deltaPhi;
        double initialPhi;

    gates:
        inout g[];
}

channel Link extends ned.DatarateChannel
{
    parameters:
        int cost = default(0);
}

network RandomNetwork
{
    @display("bgb=852,419");
    submodules:
        node0: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=347,247");
        }
        node1: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=464,268");
        }
        node2: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=299,114");
        }
        source: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=45,363");
        }
        node4: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=55,91");
        }
        node5: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=637,137");
        }
        node6: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=386,363");
        }
        node7: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=148,278");
        }
        node8: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=399,29");
        }
        node9: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=102,192");
        }
        node10: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=624,257");
        }
        node11: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=242,229");
        }
        node12: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=464,107");
        }
        node13: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=523,363");
        }
        destination: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
              deltaPhi = 2.0;
            @display("p=776,153");
        }
    connections:
        node0.g++ <--> Link <--> node1.g++;
        node0.g++ <--> Link <--> node11.g++;
        node1.g++ <--> Link <--> node2.g++;
        node1.g++ <--> Link <--> source.g++;
        node1.g++ <--> Link <--> node5.g++;
        node1.g++ <--> Link <--> node12.g++;
        node2.g++ <--> Link <--> node4.g++;
        node2.g++ <--> Link <--> node8.g++;
        source.g++ <--> Link <--> node4.g++;
        source.g++ <--> Link <--> node6.g++;
        source.g++ <--> Link <--> node7.g++;
        node1.g++ <--> Link <--> node10.g++;
        node4.g++ <--> Link <--> node8.g++;
        node4.g++ <--> Link <--> node11.g++;
        node5.g++ <--> Link <--> node8.g++;
        node5.g++ <--> Link <--> node12.g++;
        node5.g++ <--> Link <--> destination.g++;
        node6.g++ <--> Link <--> node13.g++;
        node7.g++ <--> Link <--> node9.g++;
        node7.g++ <--> Link <--> node11.g++;
        node10.g++ <--> Link <--> destination.g++;
        node13.g++ <--> node10.g++;
        node8.g++ <--> node12.g++;
}
