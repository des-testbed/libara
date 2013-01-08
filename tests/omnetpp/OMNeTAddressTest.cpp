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
#include "OMNeTAddress.h"

using namespace ARA;

TEST_GROUP(OMNeTAddressTest) {};

TEST(OMNeTAddressTest, testGetAddress) {
    OMNeTAddress address = OMNeTAddress(123);
    CHECK(address.getAddress() == 123);
}

TEST(OMNeTAddressTest, testEquality) {
    OMNeTAddress address1 = OMNeTAddress(123);
    CHECK(address1.equals(&address1));

    OMNeTAddress sameAddress = OMNeTAddress(123);
    CHECK(address1.equals(&sameAddress));

    OMNeTAddress address2 = OMNeTAddress(456);
    CHECK(address1.equals(&address2) == false);
}

TEST(OMNeTAddressTest, testEqualityWithNull) {
    OMNeTAddress address = OMNeTAddress(123);
    CHECK(address.equals(0) == false);
}

TEST(OMNeTAddressTest, testGetHashValue) {
    OMNeTAddress address = OMNeTAddress(123);
    LONGS_EQUAL(123, address.getHashValue());
}

TEST(OMNeTAddressTest, testIsBroadCast) {
    OMNeTAddress broadCastAddress = OMNeTAddress(OMNeTAddress::BROADCAST);
    OMNeTAddress normalAddress = OMNeTAddress(123);

    CHECK(broadCastAddress.isBroadCast() == true);
    CHECK(normalAddress.isBroadCast() == false);
}

TEST(OMNeTAddressTest, testCloneAddress) {
    OMNeTAddress original = OMNeTAddress(123);
    Address* clone = original.clone();

    CHECK(original.equals(clone));
    delete clone;
}
