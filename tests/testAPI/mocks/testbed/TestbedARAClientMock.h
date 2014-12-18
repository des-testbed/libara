/*
 * $FU-Copyright$
 */

#ifndef TESTBED_ARA_CLIENT_MOCK_H_
#define TESTBED_ARA_CLIENT_MOCK_H_

#include "Testbed.h"
#include "TestbedARAClient.h"
#include "TestbedPacketTrap.h"
#include "BasicConfiguration.h"
#include "TestbedRoutingTable.h"
#include "TestbedPacketFactory.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"

#include "testAPI/mocks/libara/time/ClockMock.h"
#include "testAPI/mocks/libara/AbstractClientMockBase.h"
#include "testAPI/mocks/libara/ExponentialEvaporationPolicyMock.h"

#include "testAPI/mocks/testbed/TestbedRoutingTableMock.h"
#include "testAPI/mocks/testbed/TestbedNetworkInterfaceMock.h"


TESTBED_NAMESPACE_BEGIN

class TestbedARAClientMock: public TestbedARAClient, public AbstractClientMockBase {
    public:
        TestbedARAClientMock();
        TestbedARAClientMock(Configuration& configuration);

        BasicConfiguration getStandardConfiguration() const;
        std::shared_ptr<TestbedPacketTrap> getPacketTrap();
//        TestbedPacketFactory* getPacketFactory() const;
        NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "localhost");
        TestbedRoutingTable* getRoutingTable();
};

TESTBED_NAMESPACE_END

#endif
