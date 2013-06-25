/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include <iostream>

#include "Testbed.h" 
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedAddressTest) {};


TEST(TestbedAddressTest, testToString) {
    TestbedAddress address(1,2,3,4,5,6);
    std::string expected = "1:2:3:4:5:6";
    STRCMP_EQUAL(expected.c_str(),address.toString().c_str());
}

TEST(TestbedAddressTest, testGetHashValue) {
    int leastSignificantBit = 6;
    int nextLeastSignificantBit = 5;
    TestbedAddress address(1,2,3,4, nextLeastSignificantBit,leastSignificantBit);
    int hashValueExpected = leastSignificantBit*256 + nextLeastSignificantBit;
    LONGS_EQUAL(hashValueExpected, address.getHashValue());
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
