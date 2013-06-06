/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "ExponentialEvaporationPolicy.h"

#include <cmath>

using namespace ARA;

TEST_GROUP(ExponentialEvaporationPolicyTest) {};

TEST(ExponentialEvaporationPolicyTest, evaporate) {
    float evaporationFactor = 0.9;
    float threshold = 0.75;
    unsigned int timeIntervalInMillis = 2000;
    ExponentialEvaporationPolicy policy = ExponentialEvaporationPolicy(evaporationFactor, timeIntervalInMillis, threshold);

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

TEST(ExponentialEvaporationPolicyTest, evaporateWithZeroTimeInterval) {
    float evaporationFactor = 0.9;
    float threshold = 0.75;
    unsigned int timeIntervalInMillis = 2000;
    ExponentialEvaporationPolicy policy = ExponentialEvaporationPolicy(evaporationFactor, timeIntervalInMillis, threshold);

    float pheromone = 10;

    pheromone = policy.evaporate(pheromone, 0);
    DOUBLES_EQUAL(10, pheromone, 0.00001);
}

TEST(ExponentialEvaporationPolicyTest, partialEvaporation) {
    float evaporationFactor = 0.9;
    float threshold = 0.75;
    unsigned int timeIntervalInMillis = 1000;
    ExponentialEvaporationPolicy policy = ExponentialEvaporationPolicy(evaporationFactor, timeIntervalInMillis, threshold);

    float pheromone = 10;
    pheromone = policy.evaporate(pheromone, 500);
    DOUBLES_EQUAL(10 * pow(evaporationFactor, 0.5), pheromone, 0.00001);
}

TEST(ExponentialEvaporationPolicyTest, sequentialVsContinuousEvaporation) {
    float evaporationFactor = 0.7;
    float threshold = 1;
    unsigned int timeIntervalInMillis = 1000;
    ExponentialEvaporationPolicy policy = ExponentialEvaporationPolicy(evaporationFactor, timeIntervalInMillis, threshold);

    float originalPheromone = 123;
    float evaporationInOneStep = policy.evaporate(originalPheromone, 10 * timeIntervalInMillis);

    float evaporationInMultipleSteps = originalPheromone;
    for (int i = 0; i < 10; ++i) {
        evaporationInMultipleSteps = policy.evaporate(evaporationInMultipleSteps, timeIntervalInMillis);
    }

    DOUBLES_EQUAL(evaporationInOneStep, evaporationInMultipleSteps, 0.00001);
}
