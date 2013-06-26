/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/AddressMock.h"

using namespace ARA;

TEST_GROUP(AddressMockTest) {};

TEST(AddressMockTest, testCreateWithoutName) {
    AddressMock mock = AddressMock();
    CHECK_EQUAL("Foo", mock.getAddress());
}

TEST(AddressMockTest, testCreateWithName) {
        AddressMock mock = AddressMock("Hello");
        CHECK_EQUAL("Hello", mock.getAddress());
}


/// test the overloaded << operator
TEST(AddressMockTest, testOutputOperator) {
    AddressMock mock = AddressMock();
    std::ostringstream stream;
    stream << mock;
    STRCMP_EQUAL(stream.str().c_str(), "Foo");
}

TEST(AddressMockTest, testEqualsToItself) {
    AddressMock mock = AddressMock();
    CHECK(mock.equals(&mock));

    mock = AddressMock("Test");
    CHECK(mock.equals(&mock));
}

TEST(AddressMockTest, testEqualsToSameMock) {
    AddressMock mock1 = AddressMock("Foo");
    AddressMock mock2 = AddressMock("Foo");
    CHECK(mock1.equals(&mock2));
    CHECK(mock2.equals(&mock1));
}

TEST(AddressMockTest, testNotEqualsToOtherMock) {
    AddressMock mock1 = AddressMock("Foo");
    AddressMock mock2 = AddressMock("Bar");
    CHECK(mock1.equals(&mock2) == false);
    CHECK(mock2.equals(&mock1) == false);
}

TEST(AddressMockTest, testGetHashValue) {
    AddressMock mock1 = AddressMock("Foo");
    AddressMock mock2 = AddressMock("Bar");
    AddressMock mock3 = AddressMock("Foo");
    CHECK(mock1.getHashValue() != mock2.getHashValue());
    CHECK(mock1.getHashValue() == mock3.getHashValue());
}

TEST(AddressMockTest, testOperatorEquals) {
    AddressMock mock1 = AddressMock("Foo");
    AddressMock mock2 = AddressMock("Bar");
    AddressMock mock3 = AddressMock("Foo");
    CHECK((mock1 == mock3) == true);
    CHECK((mock2 == mock1) == false);
}

TEST(AddressMockTest, toString) {
    AddressMock address = AddressMock("192.168.0.1");
    STRCMP_EQUAL("192.168.0.1", address.toString().c_str());
}
