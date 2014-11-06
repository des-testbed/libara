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

TEST(TestbedAddressTest, getHashValue) {
  u_char mac[ETHER_ADDR_LEN] = {0x50,0x51,0x52,0x04,0xf6,0xc8};
  TestbedAddress address(mac);

  int leastSignificantBit = mac[5];
  int nextLeastSignificantBit = mac[4];
  int hashValueExpected = leastSignificantBit*256 + nextLeastSignificantBit;

  LONGS_EQUAL(hashValueExpected, address.getHashValue());
}

TEST(TestbedAddressTest, equals) {
  u_char mac[ETHER_ADDR_LEN] = {0x50,0x51,0x52,0x04,0xf6,0xc8};
  u_char sameMacAddress[ETHER_ADDR_LEN] = {0x50,0x51,0x52,0x04,0xf6,0xc8};
  u_char anotherMacAddress[ETHER_ADDR_LEN] = {0x50,0x51,0x52,0x4,0xf6,0xc8};
  u_char differentMacAddress[ETHER_ADDR_LEN] = {0xfe,0x51,0x52,0x04,0xf6,0xc8};

  TestbedAddress a(mac);
  TestbedAddress b(sameMacAddress);
  TestbedAddress c(anotherMacAddress);
  TestbedAddress d(differentMacAddress);

  CHECK_TRUE(a == b);
  CHECK_TRUE(a == c);
  CHECK_TRUE(b == c);
  CHECK_FALSE(b == d);
}

TEST(TestbedAddressTest, notEquals) {
  u_char macAddress[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
  u_char otherMacAddress[ETHER_ADDR_LEN] = {15,8,2,42,17,69};
  TestbedAddress address(macAddress);
  TestbedAddress otherAddress(otherMacAddress);
  CHECK_FALSE(address == otherAddress);
}

TEST(TestbedAddressTest, getDessertValue) {
  u_char mac[ETHER_ADDR_LEN] = {0x50,0x51,0x52,0x04,0xf6,0xc8};
  TestbedAddress address(mac);
  u_int8_t* dessertAddress = address.getDessertValue();

  BYTES_EQUAL(0x50, dessertAddress[0]);
  BYTES_EQUAL(0x51, dessertAddress[1]);
  BYTES_EQUAL(0x52, dessertAddress[2]);
  BYTES_EQUAL(0x04, dessertAddress[3]);
  BYTES_EQUAL(0xf6, dessertAddress[4]);
  BYTES_EQUAL(0xc8, dessertAddress[5]);
}


TESTBED_NAMESPACE_END
