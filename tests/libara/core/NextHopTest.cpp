/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "NextHop.h"
#include "testAPI/mocks/libara/AddressMock.h"
#include "testAPI/mocks/libara/NetworkInterfaceMock.h"
#include "testAPI/mocks/libara/ARAClientMock.h"
#include <memory>

using namespace ARA;

TEST_GROUP(NextHopTest) {};

TEST(NextHopTest, testGetters) {
    ARAClientMock client = ARAClientMock();
    NetworkInterfaceMock interface = NetworkInterfaceMock(&client);
    std::shared_ptr<Address> address (new AddressMock());
    NextHop nextHop = NextHop(address, &interface);

    CHECK(nextHop.getAddress()->equals(address));
    CHECK(nextHop.getInterface()->equals(&interface));
}
