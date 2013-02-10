/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

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
