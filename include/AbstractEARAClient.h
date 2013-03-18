/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_EARA_CLIENT_H_
#define ABSTRACT_EARA_CLIENT_H_

#include "AbstractARAClient.h"
#include "TimeoutEventListener.h"
#include "Logger.h"
#include "Address.h"
#include "NextHop.h"
#include "NetworkInterface.h"
#include "PacketTrap.h"
#include "EnergyAwareRoutingTable.h"
#include "Packet.h"
#include "ForwardingPolicy.h"
#include "PathReinforcementPolicy.h"
#include "RouteDiscoveryInfo.h"
#include "Timer.h"
#include "EnergyAwareRoutingTable.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <memory>
#include <string>

namespace ARA {
    class AbstractEARAClient : public AbstractARAClient {
        public:
            AbstractEARAClient();
            virtual ~AbstractEARAClient();
            void initialize(Configuration &configuration);

        protected:
            void setupRoutingTable(); 
};

} /* namespace ARA */
#endif /* ABSTRACT_EARA_CLIENT_H_ */
