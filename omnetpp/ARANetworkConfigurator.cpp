/*
 * $FU-Copyright$
 */

#include "omnetpp/ARANetworkConfigurator.h"
#include "IPvXAddressResolver.h"
#include "IPv4InterfaceData.h"
#include "IPv4Address.h"

#include <vector>

using namespace std;

OMNETARA_NAMESPACE_BEGIN

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
    for (int i=1; i < nrOfNodes; i++) {
        if (nodeInfo[i].hasInterfaceTable == false) {
            EV << "Skipping node " << i;
            continue;
        }
        else {
            assignAddressToNode(i, ++numberOfAssignedNodes, networkAddress);
        }
    }

    // assign node[0] at last so every other node[i] has an address like 192.168.0.i
    assignAddressToNode(0, ++numberOfAssignedNodes, networkAddress);
}

void ARANetworkConfigurator::assignAddressToNode(unsigned int i, unsigned int n, uint32 networkAddress) {
    uint32 address = networkAddress | uint32(n);
    nodeInfo[i].address.set(address);

    // find interface table and assign address to all (non-loopback) interfaces
    IInterfaceTable* interfaceTable = nodeInfo[i].interfaceTable;
    for (int k=0; k < interfaceTable->getNumInterfaces(); k++)         {
        InterfaceEntry* interfaceEntry = interfaceTable->getInterface(k);
        if (interfaceEntry->isLoopback() == false) {
            IPv4Address newIPv4Address = IPv4Address(address);
            interfaceEntry->ipv4Data()->setIPAddress(newIPv4Address);
            interfaceEntry->ipv4Data()->setNetmask(IPv4Address::ALLONES_ADDRESS); // full address must match for local delivery
            EV << "Assigning IP " << newIPv4Address << " to node " << i << "\n";
        }
    }
}

void ARANetworkConfigurator::handleMessage(cMessage *msg) {
    throw cRuntimeError("The ARA::NetworkConfigurator does not use handle messages. Please do not connect anything directly to this module.");
}

OMNETARA_NAMESPACE_END
