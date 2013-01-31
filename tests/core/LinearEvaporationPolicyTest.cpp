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

#include "LinearEvaporationPolicy.h" 
#include "NoRemainingPheromoneException.h"

using namespace ARA;

TEST_GROUP(LinearEvaporationPolicyTest) {};

TEST(LinearEvaporationPolicyTest, testEvaporate) {
    LinearEvaporationPolicy policy = LinearEvaporationPolicy();

    float pheromone = 1;

    // simply test the evaporate function
    pheromone = policy.evaporate(pheromone, 1);
    DOUBLES_EQUAL(0.9, pheromone, 0.00001);

    // check the pow function used in the evaporate method
    pheromone = policy.evaporate(pheromone, 2);
    DOUBLES_EQUAL(0.6561, pheromone, 0.00001);

    pheromone = policy.evaporate(pheromone, 6);
    DOUBLES_EQUAL(0.0, pheromone, 0.00001);
}
