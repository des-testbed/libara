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
#include "testAPI/mocks/AddressMock.h"

using namespace ARA;

SUITE(AddressMockTest) {

    TEST(testCreateWithoutName) {
        AddressMock mock = AddressMock();
        CHECK_EQUAL("Foo", mock.getAddress());
    }

    TEST(testCreateWithName) {
            AddressMock mock = AddressMock("Hello");
            CHECK_EQUAL("Hello", mock.getAddress());
        }

    TEST(testEqualsToItself) {
        AddressMock mock = AddressMock();
        CHECK(mock.equals(&mock));

        mock = AddressMock("Test");
        CHECK(mock.equals(&mock));
    }

    TEST(testEqualsToSameMock) {
        AddressMock mock1 = AddressMock("Foo");
        AddressMock mock2 = AddressMock("Foo");
        CHECK(mock1.equals(&mock2));
        CHECK(mock2.equals(&mock1));
    }

    TEST(testNotEqualsToOtherMock) {
        AddressMock mock1 = AddressMock("Foo");
        AddressMock mock2 = AddressMock("Bar");
        CHECK(mock1.equals(&mock2) == false);
        CHECK(mock2.equals(&mock1) == false);
    }

    TEST(testGetHashValue) {
        AddressMock mock1 = AddressMock("Foo");
        AddressMock mock2 = AddressMock("Bar");
        AddressMock mock3 = AddressMock("Foo");
        CHECK(mock1.getHashValue() != mock2.getHashValue());
        CHECK(mock1.getHashValue() == mock3.getHashValue());
    }

    TEST(testOperatorEquals) {
        AddressMock mock1 = AddressMock("Foo");
        AddressMock mock2 = AddressMock("Bar");
        AddressMock mock3 = AddressMock("Foo");
        CHECK((mock1 == mock3) == true);
        CHECK((mock2 == mock1) == false);
    }
  }
