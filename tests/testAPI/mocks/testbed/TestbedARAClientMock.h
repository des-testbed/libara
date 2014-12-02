/*
 * $FU-Copyright$
 */

#ifndef TESTBED_ARA_CLIENT_MOCK_H_
#define TESTBED_ARA_CLIENT_MOCK_H_

#include "Testbed.h"
#include "TestbedARAClient.h"
#include "TestbedPacketTrap.h"
#include "BasicConfiguration.h"

#include "testAPI/mocks/libara/AbstractClientMockBase.h"


TESTBED_NAMESPACE_BEGIN

class TestbedARAClientMock: public TestbedARAClient, public AbstractClientMockBase {
    public:
        TestbedARAClientMock();

        BasicConfiguration getStandardConfiguration() const;
};

TESTBED_NAMESPACE_END

#endif
