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
#include <cstdlib>
#include <iostream>
#include "CppUTest/TestHarness.h"

#include "Time.h"

using namespace ARA;

TEST_GROUP(TimeTest) {};

TEST(TimeTest, testInitialize) {
    Time time = Time();
    bool result = time.isInitialized();
    CHECK(!result);
    time.initialize();
    result = time.isInitialized();
    CHECK(result);
}


/**
 * This unit test tests the operator- and operator-=
 * methods which are overloaded in class Time.
 */
TEST(TimeTest, testOperator) {
    struct timeval timestampA;
    timestampA.tv_sec = 10;
    timestampA.tv_usec = 10000;

    struct timeval timestampB;
    timestampB.tv_sec = 20;
    timestampB.tv_usec = 20000;

    Time timeA = Time(timestampA); 
    Time timeB = Time(timestampB);

    Time result = timeB - timeA;
    BYTES_EQUAL(10, result.toSeconds()); 
    LONGS_EQUAL(10000, result.toMilliseconds()); 
}

TEST(TimeTest, testUpdate) {
    Time time = Time();
    CHECK(time.getTimestamp().tv_sec == 0);
    time.update();
    CHECK(time.getTimestamp().tv_sec != 0);

    struct timeval timestamp;
    timestamp.tv_sec = 10;
    timestamp.tv_usec = 10000;

    Time anotherTime = Time(timestamp);
    time.update(anotherTime);
    CHECK(time.getTimestamp().tv_sec == 10);
}
