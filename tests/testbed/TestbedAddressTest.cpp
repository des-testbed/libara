/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"

#include "Testbed.h"
#include "TestbedMain.h"
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedAddressTest){};

TEST(TestbedAddressTest, toString) {
    u_char mac[ETHER_ADDR_LEN] = {0x50,0x51,0x52,0x04,0xf6,0xc8};
    TestbedAddress address(mac);
    std::string expected = "50:51:52:04:f6:c8";
    STRCMP_EQUAL(expected.c_str(), address.toString().c_str());

    u_char anotherMac[ETHER_ADDR_LEN] = {0x50,0x51,0x52,0x4,0xf6,0xc8};
    TestbedAddress anotherAddress(anotherMac);
    STRCMP_EQUAL(expected.c_str(), anotherAddress.toString().c_str());
}

TESTBED_NAMESPACE_END
