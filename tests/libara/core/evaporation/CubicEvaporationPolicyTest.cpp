/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "CubicEvaporationPolicy.h"

#include <cmath>
#include <climits>

using namespace ARA;

TEST_GROUP(CubicEvaporationPolicyTest) {};

/**
 * This test checks that after zero seconds since last traffic, pheromone is unchanged.
 */
TEST(CubicEvaporationPolicyTest, noEvaporateifNoTimeHasPassed) {
    unsigned int timeUntilPlateauCenterIsReached = 100;
    float plateauCenterDepth =  30;
    unsigned int exponent = 7;

    CubicEvaporationPolicy policy = CubicEvaporationPolicy(timeUntilPlateauCenterIsReached, plateauCenterDepth, exponent);

    float oldPheromone = 99.;
    float newPheromone = policy.evaporate(oldPheromone, 0);
    DOUBLES_EQUAL(oldPheromone, newPheromone, 0);
}

/**
 * This test checks that at desired plateau evaporation center is reached after the set time.
 */
TEST(CubicEvaporationPolicyTest, evaporationPlateauIsReachedAtRightTime) {
    unsigned int timeUntilPlateauCenterIsReached = 100;
    float plateauCenterDepth =  30;
    unsigned int exponent = 7;

    CubicEvaporationPolicy policy = CubicEvaporationPolicy(timeUntilPlateauCenterIsReached, plateauCenterDepth, exponent);

    float oldPheromone = 123.8;
    float newPheromone = policy.evaporate(oldPheromone, timeUntilPlateauCenterIsReached);
    DOUBLES_EQUAL(oldPheromone-plateauCenterDepth, newPheromone, 0.00001);
}

TEST(CubicEvaporationPolicyTest, evaporationContinuesAfterPlateauHasBeenReached) {
    //Test checks that after omega milliseconds, pheromone continues decaying
    unsigned int timeUntilPlateauCenterIsReached = 100;
    float plateauCenterDepth =  30;
    unsigned int exponent = 7;

    CubicEvaporationPolicy policy = CubicEvaporationPolicy(timeUntilPlateauCenterIsReached, plateauCenterDepth, exponent);

    float oldPheromone = 68.;

    for (int offset = 20; offset <= 200; offset+=20) {
        float newPheromone = policy.evaporate(oldPheromone, timeUntilPlateauCenterIsReached + offset);
        CHECK(newPheromone <= (oldPheromone - plateauCenterDepth));
    }

}

/**
 * This test checks that the pheromone is within precision of plateau near upper corner.
 */
TEST(CubicEvaporationPolicyTest, evaporateUpperPlateauCorner){
    unsigned int alpha =  63;
    float plateauCenterDepth = 47.2;
    unsigned int exponent = 3;

    CubicEvaporationPolicy policy = CubicEvaporationPolicy(alpha, plateauCenterDepth, exponent);

    float oldPheromone = 123.8;
    float precision = 0.01;

    // using Friedrich's equation 3.13, pow is used to calculate the root
    float t1 = alpha + pow( (pow(alpha, exponent) * precision) / plateauCenterDepth, (1.0/exponent));
    float newPheromone = policy.evaporate(oldPheromone, t1);

    // phrased this way such that floating point values don't cause erroneous failure
    CHECK((newPheromone-(oldPheromone-plateauCenterDepth))- precision <= .0001);
}

/**
 * This test checks that pheromone is within precision of plateau near upper corner.
 */
TEST(CubicEvaporationPolicyTest, evaporateLowerPlateauCorner){
    unsigned int alpha =  63;
    float plateauCenterDepth = 47.2;
    unsigned int exponent = 3;

    CubicEvaporationPolicy policy = CubicEvaporationPolicy(alpha, plateauCenterDepth, exponent);

    float oldPheromone = 123.8;
    float precision = 0.001;

    //Using Friedrich's equation 3.13; compiler can't handle negative odd roots, so moved negative
    //mathematically gives the negative real root without tripping compiler
    float t1 = alpha - pow((pow(alpha,exponent)*precision/plateauCenterDepth),(1.0/exponent));
    float newPheromone = policy.evaporate(oldPheromone, t1);
    DOUBLES_EQUAL((oldPheromone-plateauCenterDepth), newPheromone, precision);
}

/**
 * This test checks that at desired plateau center evaporation is at desired plateau value.
 */
TEST(CubicEvaporationPolicyTest, testLowerPheromoneBound){
    unsigned int timeUntilPlateauCenterIsReached =  630;
    float plateauCenterDepth = 47.2;
    unsigned int exponent = 11;

    CubicEvaporationPolicy policy = CubicEvaporationPolicy(timeUntilPlateauCenterIsReached, plateauCenterDepth, exponent);

    float oldPheromone = 123.8;
    unsigned int unbelievablyLong = UINT_MAX;

    float newPheromone = policy.evaporate(oldPheromone, unbelievablyLong);

    DOUBLES_EQUAL(0, newPheromone, 0.00001);
}
