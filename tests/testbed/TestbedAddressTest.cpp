#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include <iostream>

#include "Testbed.h" 
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedAddressTest) {};

TEST(TestbedAddressTest, testToString) {
    TestbedAddress address(1,2,3,4,5,6);
    std::cout << address.toString() << std::endl;
}

TESTBED_NAMESPACE_END
