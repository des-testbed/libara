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

#include <UnitTest++.h>
#include <string>
#include "LinkedList.h"
#include "NoSuchElementException.h"

using namespace ARA;
using namespace std;

SUITE(LinkedListTest) {

    TEST(testCreate) {
        LinkedList<string> list = LinkedList<string>();
        CHECK(list.isEmpty() == true);
    }

    TEST(testAddAndGet) {
        LinkedList<string> list = LinkedList<string>();
        string foo = string("Hello Foo World");
        string bar = string("Hello Bar World");
        string fooBar = string("Hello FooBar World");

        list.add(&foo);
        list.add(&bar);
        list.add(&fooBar);

        CHECK_EQUAL(foo, *list.get(0));
        CHECK_EQUAL(fooBar, *list.get(2));
        CHECK_EQUAL(bar, *list.get(1));
    }

    TEST(testGetThrowsExceptionIfIndexIsOutOfBounds) {
        LinkedList<string> list = LinkedList<string>();
        CHECK_THROW(list.get(0), ARA::Exception);

        string foo = string("Hello Foo World");
        list.add(&foo);
        list.get(0);    // This should now work

        string bar = string("Hello Bar World");
        list.add(&bar);
        list.get(1);    // This should now work

        CHECK_THROW(list.get(2), Exception);
    }

    TEST(testIsEmpty) {
        LinkedList<string> list = LinkedList<string>();
        string test = string("Hello World");

        CHECK(list.isEmpty() == true);
        list.add(&test);
        CHECK(list.isEmpty() == false);
    }

    TEST(DeletingFromEmtyListthrowsException) {
        LinkedList<string> list = LinkedList<string>();
        CHECK_THROW(list.remove(), NoSuchElementException);
    }

    TEST(testRemove) {
        LinkedList<string> list = LinkedList<string>();
        string foo = string("Foo");
        string bar = string("Bar");
        string hello = string("Hello World");

        list.add(&foo);
        list.add(&bar);
        list.add(&hello);

        CHECK(list.isEmpty() == false);

        CHECK_EQUAL(foo, *list.remove());
        CHECK_EQUAL(bar, *list.remove());
        CHECK_EQUAL(hello, *list.remove());

        CHECK(list.isEmpty() == true);
    }

    TEST(testSize) {
        LinkedList<string> list = LinkedList<string>();
        string foo = string("Foo");
        string bar = string("Bar");

        CHECK_EQUAL(0, list.size());

        list.add(&foo);
        CHECK_EQUAL(1, list.size());

        list.add(&bar);
        CHECK_EQUAL(2, list.size());

        list.remove();
        CHECK_EQUAL(1, list.size());

        list.remove();
        CHECK_EQUAL(0, list.size());
    }

    TEST(testContains) {
        LinkedList<string> list = LinkedList<string>();
        string foo = string("Foo");
        string bar = string("Bar");

        CHECK(list.contains(&foo) == false);
        CHECK(list.contains(&bar) == false);

        list.add(&foo);
        CHECK(list.contains(&foo) == true);
        CHECK(list.contains(&bar) == false);

        list.add(&bar);
        CHECK(list.contains(&foo) == true);
        CHECK(list.contains(&bar) == true);

        list.remove();
        CHECK(list.contains(&foo) == false);
        CHECK(list.contains(&bar) == true);

        list.remove();
        CHECK(list.contains(&foo) == false);
        CHECK(list.contains(&bar) == false);
    }
}
