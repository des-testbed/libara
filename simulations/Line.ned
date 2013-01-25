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
import ara_simulation.simulations.OMNeTARAClient;

import Link;

//
// Generated network with random topology (8 nodes, 8 edges, seed=100).
//
network Line
{
    @display("bgb=900,327;bgl=2");
    submodules:
        node0: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=689,132");
        }
        node1: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=217,132");
        }
        node2: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=122,131");
        }
        node3: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=322,131");
        }
        source: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=42,132");
        }
        node5: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=574,132");
        }
        destination: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=787,131");
        }
        node7: OMNeTARAClient {
            parameters:
              policy = "BestPheromoneForwardingPolicy";
            @display("p=466,132");
        }
    connections:
        node0.g++ <--> Link <--> node5.g++;
        node0.g++ <--> Link <--> destination.g++;
        node1.g++ <--> Link <--> node2.g++;
        node1.g++ <--> Link <--> node3.g++;
        node7.g++ <--> Link <--> node5.g++;
        node2.g++ <--> Link <--> source.g++;
        node3.g++ <--> Link <--> node7.g++;
}
