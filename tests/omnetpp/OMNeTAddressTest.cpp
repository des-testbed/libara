#include "CppUTest/TestHarness.h"
#include "IPAddress.h"
#include "OMNeTAddress.h"

using namespace ARA::omnetpp;

TEST_GROUP(OMNeTAddressTest) {};

TEST(OMNeTAddressTest, testCreateFromIPAddress) {
    IPAddress ipAddress = IPAddress(192, 168, 0, 1);
    OMNeTAddress omnetAddress = OMNeTAddress(ipAddress);

    CHECK(omnetAddress.equals(ipAddress));
    CHECK(ipAddress.equals(omnetAddress));
}
