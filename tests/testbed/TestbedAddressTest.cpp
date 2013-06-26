/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include <iostream>

#include "Testbed.h" 
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedAddressTest) {
    u_char macAddress1[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    u_char macAddress2[ETHER_ADDR_LEN] = {15,8,2,42,17,69};
    u_char macAddress3[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
};


TEST(TestbedAddressTest, testToString) {
    TestbedAddress address(macAddress1);
    std::string expected = "1:2:3:4:5:6";
    STRCMP_EQUAL(expected.c_str(),address.toString().c_str());
}

TEST(TestbedAddressTest, testGetHashValue) {
    int leastSignificantBit = macAddress1[5];
    int nextLeastSignificantBit = macAddress1[4];
    TestbedAddress address(macAddress1);
    int hashValueExpected = leastSignificantBit*256 + nextLeastSignificantBit;
    LONGS_EQUAL(hashValueExpected, address.getHashValue());
}

TEST(TestbedAddressTest, testEquals) {
    TestbedAddress address1(macAddress1);
    TestbedAddress address2(macAddress3);
    CHECK_TRUE(address1==address2);
}

TEST(TestbedAddressTest, testClone) {
    TestbedAddress address1(macAddress1);
    Address* address2 = address1.clone();
    CHECK_TRUE(address1==*address2);
    delete address2;
}

TEST(TestbedAddressTest, testNotEquals) {
    TestbedAddress address1(macAddress1);
    TestbedAddress address2(macAddress2);
    CHECK_FALSE(address1==address2);
}

TESTBED_NAMESPACE_END
