/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include <cstring>
#include "testAPI/mocks/PacketMock.h"
#include "testAPI/mocks/AddressMock.h"

using namespace ARA;

TEST_GROUP(PacketMockTest) {};

TEST(PacketMockTest, testConstructor) {
    PacketMock mock = PacketMock("Source", "Destination", 123, 15);

    AddressMock wantedSource = AddressMock("Source");
    AddressMock wantedDestination = AddressMock("Destination");
    AddressMock wantedSender = wantedSource;

    CHECK(mock.getSource()->equals(&wantedSource));
    CHECK(mock.getDestination()->equals(&wantedDestination));
    CHECK(mock.getSender()->equals(&wantedSender));
    CHECK_EQUAL(123, mock.getSequenceNumber());
    CHECK_EQUAL(15, mock.getTTL());
    STRCMP_EQUAL("Hello World", mock.getPayload());
    LONGS_EQUAL(strlen("Hello World")+1, mock.getPayloadLength());
}

TEST(PacketMockTest, testSetSender) {
    PacketMock mock = PacketMock("A", "B");
    CHECK(mock.getSender()->equals(mock.getSource()));

    std::shared_ptr<Address> newSender (new AddressMock("C"));
    mock.setSender(newSender);

    CHECK(mock.getSender()->equals(newSender));
}
