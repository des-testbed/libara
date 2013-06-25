#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include <iostream>

#include "Testbed.h" 
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedAddressTest) {};


TEST(TestbedAddressTest, testToString) {
    TestbedAddress address(1,2,3,4,5,6);
    std::cout << "Address 1:2:3:4:5:6 "<< address.toString() << std::endl;
}

TEST(TestbedAddressTest, testGetHashValue) {
    TestbedAddress address(1,2,3,4,5,6);
    std::cout << "Hash of address "<< address.getHashValue() << std::endl;
}

TEST(TestbedAddressTest, testEquals) {
    TestbedAddress address1(6,5,4,3,2,1);
    TestbedAddress address2(6,5,4,3,2,1);
    CHECK_TRUE(address1==address2);
}

TEST(TestbedAddressTest, testNotEquals) {
    TestbedAddress address1(6,5,4,3,2,1);
    TestbedAddress address2(1,2,3,4,5,6);
    CHECK_FALSE(address1==address2);
}

TESTBED_NAMESPACE_END
