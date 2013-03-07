/*
 * $FU-Copyright$
 */

#include <iostream>
#include <unistd.h>
#include "CppUTest/TestHarness.h"

#include "LinearEvaporationPolicy.h" 
#include "testAPI/mocks/time/TimeFactoryMock.h"

using namespace ARA;

TEST_GROUP(LinearEvaporationPolicyTest) {};

TEST(LinearEvaporationPolicyTest, evaporate) {
    float evaporationFactor = 0.9;
    float threshold = 0.75;
    unsigned int timeIntervalInMillis = 2000;
    LinearEvaporationPolicy policy = LinearEvaporationPolicy(evaporationFactor, threshold, timeIntervalInMillis);

    float pheromone = 1;

    // nothing should happen if no time has passed
    pheromone = policy.evaporate(pheromone, 0);
    DOUBLES_EQUAL(1.0, pheromone, 0.00001);

    // first linear evaporation
    pheromone = policy.evaporate(pheromone, timeIntervalInMillis);
    DOUBLES_EQUAL(0.9, pheromone, 0.00001);

    // another linear evaporation
    pheromone = policy.evaporate(pheromone, timeIntervalInMillis);
    DOUBLES_EQUAL(0.81, pheromone, 0.00001);

    // check if the threshold sets the pheromone value to null
    pheromone = policy.evaporate(pheromone, timeIntervalInMillis);
    DOUBLES_EQUAL(0.0, pheromone, 0.00001);
}

TEST(LinearEvaporationPolicyTest, evaporateWithZeroTimeInterval) {
    float evaporationFactor = 0.9;
    float threshold = 0.75;
    unsigned int timeIntervalInMillis = 2000;
    LinearEvaporationPolicy policy = LinearEvaporationPolicy(evaporationFactor, threshold, timeIntervalInMillis);

    float pheromone = 10;

    pheromone = policy.evaporate(pheromone, 0);
    DOUBLES_EQUAL(10, pheromone, 0.00001);
}
