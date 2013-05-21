/*
 * $FU-Copyright$
 */


#include "CppUTest/TestHarness.h"
#include <memory>

/**
 * A sample class for the SharedPointerTest::testUsage
 */
bool destructorOfAnyFooHasBeenCalled;
struct Foo {
    Foo( int x ) : x(x) {}
    ~Foo() { destructorOfAnyFooHasBeenCalled = true; }
    int getValue() { return x;}
    int x;
};

TEST_GROUP(SharedPointerTest) {};

TEST(SharedPointerTest, testUsage) {
    std::shared_ptr<Foo> p1;
    destructorOfAnyFooHasBeenCalled = false;
    { // extra scope for testing
        p1 = std::shared_ptr<Foo>(new Foo(123));
        std::shared_ptr<Foo> p2 (p1); // per copy constructor
        std::shared_ptr<Foo> p3 = p2; // per assignment

        // We can still use p1, p2, p3 as if they where ordinary pointers to a Foo instance
        LONGS_EQUAL(123, p1->getValue());
        LONGS_EQUAL(123, p2->getValue());
        LONGS_EQUAL(123, p3->getValue());
        CHECK(destructorOfAnyFooHasBeenCalled == false);

        // Make p2 point to another instance of Foo
        p2.reset(new Foo(456));

        LONGS_EQUAL(123, p1->getValue());
        LONGS_EQUAL(456, p2->getValue());
        LONGS_EQUAL(123, p3->getValue());
        CHECK(destructorOfAnyFooHasBeenCalled == false);

        // Delete p1 so only p3 is pointing to the original Foo instance
        p1.reset();
        LONGS_EQUAL(456, p2->getValue());
        LONGS_EQUAL(123, p3->getValue());
        CHECK(destructorOfAnyFooHasBeenCalled == false);
    }
    // Delete the last reference to the first Foo instance (p3) by going out of scope
    CHECK(destructorOfAnyFooHasBeenCalled == true);
}
