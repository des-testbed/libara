/*
 * $FU-Copyright$
 */

#include "omnetpp/ARANetworkConfigurator.h"
#include "IPvXAddressResolver.h"
#include "IPv4InterfaceData.h"
#include "IPv4Address.h"
#include "FWMath.h"

#include <vector>
#include <regex>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

OMNETARA_NAMESPACE_BEGIN

Define_Module(ARANetworkConfigurator);

void ARANetworkConfigurator::initialize(int stage) {
    if (stage==2) {
        channelControl = check_and_cast<cModule*>(simulation.getSystemModule()->getModuleByRelativePath("channelControl"));
        cTopology topology ("topology");
        extractTopology(topology);
        assignAddresses(topology);
        if(par("persistStartPositions").boolValue() == true) {
            persistStartPositions(topology);
        }
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
        nodeInfo[i].name = module->getFullName();
        if(nodeInfo[i].name.back() == ']') {
            nodeInfo[i].name = nodeInfo[i].name.substr(0, nodeInfo[i].name.length()-1);
            nodeInfo[i].isVectorNode = true;
        }

        nodeInfo[i].hasInterfaceTable = IPvXAddressResolver().findInterfaceTableOf(module)!=NULL;
        if (nodeInfo[i].hasInterfaceTable) {
            nodeInfo[i].interfaceTable = IPvXAddressResolver().interfaceTableOf(module);
        }

        nodeInfo[i].mobility = ModuleAccess<IMobility>("mobility").get(module);
        nodeInfo[i].radio = ModuleAccess<Radio>("radio").get(module);
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

    int indexOfNodeNumberZero = -1;
    int nrOfAssignedAddresses = 0;
    for (int i=0; i < nrOfNodes; i++) {
        if (nodeInfo[i].hasInterfaceTable == false) {
            EV << "Skipping node " << i;
            continue;
        }
        else {
            int nodeNumber = extractNodeNumber(i);
            if(nodeNumber != 0) {
                assignAddressToNode(i, nodeNumber, networkAddress);
                nrOfAssignedAddresses++;
            }
            else {
                // vector nodes start with index 0 but we do not want a node
                // to get the standard network address so we assign this later
                indexOfNodeNumberZero = i;
            }
        }
    }

    if(indexOfNodeNumberZero >= 0) {
        assignAddressToNode(indexOfNodeNumberZero, ++nrOfAssignedAddresses, networkAddress);
    }
}

int ARANetworkConfigurator::extractNodeNumber(int i) {
    size_t last_index = nodeInfo[i].name.find_last_not_of("0123456789");
    string result = nodeInfo[i].name.substr(last_index + 1);
    if(result.empty()) {
        throw cRuntimeError("Can not parse node name: %s. Please let all node names end with an integer!", nodeInfo[i].name.c_str());
    }

    return atoi(result.c_str());
}

void ARANetworkConfigurator::assignAddressToNode(unsigned int i, unsigned int n, uint32 networkAddress) {
    uint32 address = networkAddress | uint32(n);
    nodeInfo[i].address.set(address);

    // find interface table and assign address to all (non-loopback) interfaces
    IInterfaceTable* interfaceTable = nodeInfo[i].interfaceTable;
    for (int k=0; k < interfaceTable->getNumInterfaces(); k++) {
        InterfaceEntry* interfaceEntry = interfaceTable->getInterface(k);
        if (interfaceEntry->isLoopback() == false) {
            IPv4Address newIPv4Address = IPv4Address(address);
            interfaceEntry->ipv4Data()->setIPAddress(newIPv4Address);
            interfaceEntry->ipv4Data()->setNetmask(IPv4Address::ALLONES_ADDRESS); // full address must match for local delivery
            EV << "Assigning IP " << newIPv4Address << " to node " << nodeInfo[i].name << (nodeInfo[i].isVectorNode ? "]" : "") << endl;
            ipMACMapping[newIPv4Address] = interfaceEntry->getMacAddress();
        }
    }
}

void ARANetworkConfigurator::handleMessage(cMessage *msg) {
    throw cRuntimeError("The ARA::NetworkConfigurator does not use handle messages. Please do not connect anything directly to this module.");
}

MACAddress ARANetworkConfigurator::getMACAddressByIP(IPv4Address address) {
    return ipMACMapping[address];
}

IPv4Address ARANetworkConfigurator::getIPAddressFromMAC(MACAddress address) {
    std::unordered_map<IPv4Address, MACAddress, IPv4AddressHash, IPv4AddressPredicate>::iterator iterator;
    for (iterator=ipMACMapping.begin(); iterator!=ipMACMapping.end(); iterator++) {
        if(iterator->second.equals(address)) {
            return iterator->first;
        }
    }

    throw cRuntimeError("Can not find IPv4 address to given MAC address");
}

void ARANetworkConfigurator::persistStartPositions(cTopology& topology) {
    mkdir("results", 0777);
    stringstream fileName;
    ofstream file;
    cConfigurationEx* config = ev.getConfigEx();
    fileName << "results/" << config->getActiveConfigName() << "-" << config->getActiveRunNumber() << "-StartPositions.net";
    file.open(fileName.str());

    int nrOfNodes = topology.getNumNodes();
    for (int i=0; i < nrOfNodes; i++) {
        Coord coordinations = nodeInfo[i].mobility->getCurrentPosition();
        double receptionRadius = calculateMaximumRadioReceptionRadius(nodeInfo[i].radio);
        file << nodeInfo[i].name << (nodeInfo[i].isVectorNode ? "] " : " ");
        file << coordinations.x << " " << coordinations.y << " " << receptionRadius << endl;
    }
    file.close();
}

/**
 * Mostly copied from inetmanet/src/linklayer/radio/Radio.cc Lines 991ff
 */
double ARANetworkConfigurator::calculateMaximumRadioReceptionRadius(Radio* radio) {
    double carrierFrequency = channelControl->par("carrierFrequency");
    double alpha = channelControl->par("alpha");
    double waveLength = (SPEED_OF_LIGHT / carrierFrequency);
    double transmitterPower = radio->par("transmitterPower");
    double receiverPower = FWMath::dBm2mW(radio->par("sensitivity"));

    return pow(waveLength * waveLength * transmitterPower / (16.0 * M_PI * M_PI * receiverPower), 1.0 / alpha);
}

OMNETARA_NAMESPACE_END
