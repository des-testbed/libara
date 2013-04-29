/*
 * $FU-Copyright$
 */

#ifndef __ARA_SIMULATION_NETWORKCONFIGURATOR_H_
#define __ARA_SIMULATION_NETWORKCONFIGURATOR_H_

#include "OMNeTARAMacros.h"
#include "IPv4Address.h"
#include "MACAddress.h"
#include "IInterfaceTable.h"

#include <omnetpp.h>
#include <string>
#include <unordered_map>

OMNETARA_NAMESPACE_BEGIN

/**
 * This Functor is needed for std::unordered_map (hashmap implementation)
 */
struct IPv4AddressHash {
    size_t operator()(IPv4Address address) const {
        int firstByte = address.getDByte(0);
        int secondByte = address.getDByte(1);
        return firstByte * 256 + secondByte;  // integer value between 0 and 65535
    }
};

/**
 * This Functor is needed for std::unordered_map (hashmap implementation)
 */
struct IPv4AddressPredicate {
    size_t operator()(IPv4Address address1, IPv4Address address2) const {
        return address1.equals(address2);
    }
};

/**
 * The NetworkConfigurator assigns IPv4 addresses to all nodes in the network.
 *
 * Unlike the INET/MANET NetworkConfigurator classes its does not search for
 * any routes or update any routing tables.
 */
class ARANetworkConfigurator : public cSimpleModule {
public:
    MACAddress getMACAddressByIP(IPv4Address address);

protected:
    struct NodeInfo {
        NodeInfo() {hasInterfaceTable=false; interfaceTable=NULL;}
        bool hasInterfaceTable;
        std::string name;
        IInterfaceTable* interfaceTable;
        IPv4Address address;
        bool isVectorNode = false;
    };
    typedef std::vector<NodeInfo> NodeInfoVector;

    virtual int numInitStages() const  {return 3;}
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);

    void extractTopology(cTopology& topo);
    void assignAddresses(cTopology& topology);
    void assignAddressToNode(unsigned int i, unsigned int n, uint32 networkAddress);
    int extractNodeNumber(int i);

private:
    NodeInfoVector nodeInfo;
    std::unordered_map<IPv4Address, MACAddress, IPv4AddressHash, IPv4AddressPredicate> ipMACMapping;

};

OMNETARA_NAMESPACE_END

#endif
