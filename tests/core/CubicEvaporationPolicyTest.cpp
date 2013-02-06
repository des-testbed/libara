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

#include <iostream>
#include <unistd.h>
#include "CppUTest/TestHarness.h"

#include "CubicEvaporationPolicy.h" 

using namespace ARA;

TEST_GROUP(CubicEvaporationPolicyTest) {};

IGNORE_TEST(CubicEvaporationPolicyTest, testEvaporate) {
    CubicEvaporationPolicy policy = CubicEvaporationPolicy();
    // set the interval to 200 millisecond
    policy.setInterval(200);
    // 'trigger the evaporation mechanism
    bool status = policy.checkForEvaporation();
    // the 'last access time' has not been intialized
    CHECK(!status);
    // initialize the pheromone value
    float pheromone = 1.;
    // simply test the evaporate function
    pheromone = policy.evaporate(pheromone);
    DOUBLES_EQUAL(0.9, pheromone, 0.00001);
/*
    /// sleep for 10 ms
    usleep(200);
    // check the status
    status = policy.checkForEvaporation();
    // the 'last access time' should have been intialized
    CHECK(status);
    // simply test the evaporate function
    pheromone = policy.evaporate(pheromone);
    DOUBLES_EQUAL(0.81, pheromone, 0.00001);

    // set the interval to 10 millisecond
    policy.setInterval(10);
    /// sleep for 10 ms
    usleep(200);
    // check the status
    status = policy.checkForEvaporation();
    CHECK(status);
    pheromone = policy.evaporate(pheromone);
    // check if the threshold sets the pheromone value to null
    DOUBLES_EQUAL(0.0, pheromone, 0.00001);
*/
}

