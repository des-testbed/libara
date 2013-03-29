#include "CppUTest/TestHarness.h"
#include "IPv4Address.h"
#include "OMNeTAddress.h"

using namespace ARA::omnetpp;

TEST_GROUP(OMNeTAddressTest) {};

TEST(OMNeTAddressTest, testCreateFromIPAddress) {
    IPv4Address ipAddress = IPv4Address(192, 168, 0, 1);
    OMNeTAddress omnetAddress = OMNeTAddress(ipAddress);

    CHECK(omnetAddress.equals(ipAddress));
    CHECK(ipAddress.equals(omnetAddress));
}
