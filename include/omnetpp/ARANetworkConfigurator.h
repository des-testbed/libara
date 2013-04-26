/*
 * $FU-Copyright$
 */

#ifndef __ARA_SIMULATION_NETWORKCONFIGURATOR_H_
#define __ARA_SIMULATION_NETWORKCONFIGURATOR_H_

#include "OMNeTARAMacros.h"
#include "IPv4Address.h"
#include "IInterfaceTable.h"

#include <omnetpp.h>

OMNETARA_NAMESPACE_BEGIN

/**
 * The NetworkConfigurator assigns IPv4 addresses to all nodes in the network.
 *
 * Unlike the INET/MANET NetworkConfigurator classes its does not search for
 * any routes or update any routing tables.
 */
class ARANetworkConfigurator : public cSimpleModule {
protected:
    struct NodeInfo {
        NodeInfo() {hasInterfaceTable=false; interfaceTable=NULL;}
        bool hasInterfaceTable;
        IInterfaceTable* interfaceTable;
        IPv4Address address;
    };
    typedef std::vector<NodeInfo> NodeInfoVector;

    virtual int numInitStages() const  {return 3;}
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);

    void extractTopology(cTopology& topo);
    void assignAddresses(cTopology& topology);
    void assignAddressToNode(unsigned int i, unsigned int n, uint32 networkAddress);

private:
    NodeInfoVector nodeInfo;
};

OMNETARA_NAMESPACE_END

#endif
