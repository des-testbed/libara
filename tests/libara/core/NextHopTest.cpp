/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "NextHop.h"
#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/NetworkInterfaceMock.h"
#include <memory>

using namespace ARA;

TEST_GROUP(NextHopTest) {};

TEST(NextHopTest, testGetters) {
    std::shared_ptr<Address> address (new AddressMock());
    NetworkInterfaceMock interface = NetworkInterfaceMock();
    NextHop nextHop = NextHop(address, &interface);

    CHECK(nextHop.getAddress()->equals(address));
    CHECK(nextHop.getInterface()->equals(&interface));
}
