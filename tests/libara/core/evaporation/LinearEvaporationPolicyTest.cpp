/*
 * $FU-Copyright$
 */

#include <iostream>
#include <unistd.h>
#include "CppUTest/TestHarness.h"

#include "LinearEvaporationPolicy.h" 

using namespace ARA;

TEST_GROUP(LinearEvaporationPolicyTest) {};

TEST(LinearEvaporationPolicyTest, evaporate) {
    float evaporationFactor = 0.25;
    float threshold = 0.75;
    unsigned int timeIntervalInMillis = 2000;
    LinearEvaporationPolicy policy = LinearEvaporationPolicy(evaporationFactor, threshold, timeIntervalInMillis);

    float pheromone = 1.5;

    // nothing should happen if no time has passed
    pheromone = policy.evaporate(pheromone, 0);
    DOUBLES_EQUAL(1.5, pheromone, 0.00001);

    // first linear evaporation
    pheromone = policy.evaporate(pheromone, timeIntervalInMillis);
    DOUBLES_EQUAL(1.25, pheromone, 0.00001);

    // another linear evaporation
    pheromone = policy.evaporate(pheromone, timeIntervalInMillis);
    DOUBLES_EQUAL(1.00, pheromone, 0.00001);

    // check if the threshold sets the pheromone value to null
    pheromone = policy.evaporate(pheromone, timeIntervalInMillis);
    DOUBLES_EQUAL(0.75, pheromone, 0.00001);
}

TEST(LinearEvaporationPolicyTest, evaporateWithZeroTimeInterval) {
    float evaporationFactor = 0.25;
    float threshold = 0.75;
    LinearEvaporationPolicy policy = LinearEvaporationPolicy(evaporationFactor, threshold, 1000);

    float pheromone = 10;

    pheromone = policy.evaporate(pheromone, 0);
    DOUBLES_EQUAL(10, pheromone, 0.00001);
}
