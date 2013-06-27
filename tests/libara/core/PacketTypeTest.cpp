/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "PacketType.h"

using namespace ARA;

TEST_GROUP(PacketTypeTest) {};

TEST(PacketTypeTest, testIsAntPacket) {
    CHECK(PacketType::isAntPacket(PacketType::FANT) == true);
    CHECK(PacketType::isAntPacket(PacketType::BANT) == true);
    CHECK(PacketType::isAntPacket(PacketType::PANT) == true);
    CHECK(PacketType::isAntPacket(PacketType::DATA) == false);
    CHECK(PacketType::isAntPacket(PacketType::DUPLICATE_ERROR) == false);
    CHECK(PacketType::isAntPacket(PacketType::ACK) == false);
    CHECK(PacketType::isAntPacket(PacketType::ROUTE_FAILURE) == false);
    CHECK(PacketType::isAntPacket(PacketType::HELLO) == false);
}

TEST(PacketTypeTest, testIsDataPacket) {
    CHECK(PacketType::isDataPacket(PacketType::FANT) == false);
    CHECK(PacketType::isDataPacket(PacketType::BANT) == false);
    CHECK(PacketType::isDataPacket(PacketType::PANT) == false);
    CHECK(PacketType::isDataPacket(PacketType::DATA) == true);
    CHECK(PacketType::isDataPacket(PacketType::DUPLICATE_ERROR) == false);
    CHECK(PacketType::isDataPacket(PacketType::ACK) == false);
    CHECK(PacketType::isDataPacket(PacketType::ROUTE_FAILURE) == false);
    CHECK(PacketType::isDataPacket(PacketType::HELLO) == false);
}

TEST(PacketTypeTest, testGetAsString) {
    CHECK_EQUAL("FANT", PacketType::getAsString(PacketType::FANT));
    CHECK_EQUAL("BANT", PacketType::getAsString(PacketType::BANT));
    CHECK_EQUAL("PANT", PacketType::getAsString(PacketType::PANT));
    CHECK_EQUAL("DATA", PacketType::getAsString(PacketType::DATA));
    CHECK_EQUAL("DUPLICATE_ERROR", PacketType::getAsString(PacketType::DUPLICATE_ERROR));
    CHECK_EQUAL("ACK", PacketType::getAsString(PacketType::ACK));
    CHECK_EQUAL("ROUTE_FAILURE", PacketType::getAsString(PacketType::ROUTE_FAILURE));
    CHECK_EQUAL("HELLO", PacketType::getAsString(PacketType::HELLO));
    CHECK_EQUAL("UNKOWN", PacketType::getAsString(123));
}
