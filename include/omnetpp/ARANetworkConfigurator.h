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

#ifndef __ARA_SIMULATION_NETWORKCONFIGURATOR_H_
#define __ARA_SIMULATION_NETWORKCONFIGURATOR_H_

#include "IPv4Address.h"
#include "IInterfaceTable.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

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

private:
    NodeInfoVector nodeInfo;
};

} /* namespace omnetpp */
} /* namespace ARA */

#endif
