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

#include "omnetpp/ARANetworkConfigurator.h"
#include "IPvXAddressResolver.h"
#include "IPv4InterfaceData.h"
#include "IPv4Address.h"

#include <vector>

using namespace std;

namespace ARA {
namespace omnetpp {

Define_Module(ARANetworkConfigurator);

void ARANetworkConfigurator::initialize(int stage) {
    if (stage==2) {
        cTopology topology ("topology");
        extractTopology(topology);
        assignAddresses(topology);
    }
}

void ARANetworkConfigurator::extractTopology(cTopology& topology) {
    topology.extractByProperty("node");

    EV << "ARANetworkConfigurator is configuring " << topology.getNumNodes() << " nodes\n";

    int numberOfNodes = topology.getNumNodes();

    if(numberOfNodes <= 0) {
        throw cRuntimeError("Error while setting up the network configuration: Could not find any ara.NetworkNode");
    }

    nodeInfo.resize(numberOfNodes);
    for (int i=0; i<numberOfNodes; i++) {
        cModule* module = topology.getNode(i)->getModule();
        nodeInfo[i].hasInterfaceTable = IPvXAddressResolver().findInterfaceTableOf(module)!=NULL;
        if (nodeInfo[i].hasInterfaceTable) {
            nodeInfo[i].interfaceTable = IPvXAddressResolver().interfaceTableOf(module);
        }
    }
}

void ARANetworkConfigurator::assignAddresses(cTopology& topology) {
    uint32 networkAddress = IPv4Address(par("networkAddress").stringValue()).getInt();
    uint32 netmask = IPv4Address(par("netmask").stringValue()).getInt();

    int maximumNrOfNodes = (~netmask)-1;
    int nrOfNodes = topology.getNumNodes();
    if (nrOfNodes > maximumNrOfNodes) {
        error("ARANetworkConfigurator: Netmask does not fit. There are not enough available addresses for all %d nodes", nrOfNodes);
    }

    int numberOfAssignedNodes = 0;
    for (int i=0; i < nrOfNodes; i++) {

        if (nodeInfo[i].hasInterfaceTable == false) {
            EV << "Skipping node " << i;
            continue;
        }

        uint32 address = networkAddress | uint32(++numberOfAssignedNodes);
        nodeInfo[i].address.set(address);

        // find interface table and assign address to all (non-loopback) interfaces
        IInterfaceTable* interfaceTable = nodeInfo[i].interfaceTable;
        for (int k=0; k < interfaceTable->getNumInterfaces(); k++)         {
            InterfaceEntry* interfaceEntry = interfaceTable->getInterface(k);
            if (interfaceEntry->isLoopback() == false) {
                IPv4Address newIPv4Address = IPv4Address(address);
                interfaceEntry->ipv4Data()->setIPAddress(newIPv4Address);
                interfaceEntry->ipv4Data()->setNetmask(IPv4Address::ALLONES_ADDRESS); // full address must match for local delivery
                //TODO check the line above
                EV << "Assigning IP " << newIPv4Address << " to node " << i << "\n";
            }
        }
    }
}

void ARANetworkConfigurator::handleMessage(cMessage *msg) {
    throw cRuntimeError("The ARA::NetworkConfigurator does not use handle messages. Please do not connect anything directly to this module.");
}

} /* namespace omnetpp */
} /* namespace ARA */
