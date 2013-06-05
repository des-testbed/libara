/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "CubicEvaporationPolicy.h"
#include <cmath>

using namespace ARA;

TEST_GROUP(CubicEvaporationPolicyTest) {};

TEST(CubicEvaporationPolicyTest, testEvaporate) {
    //Test checks that after omega milliseconds, pheromone continues decaying
    float plateau = 10.8;
    float center =  99.8;
    int degree = 7;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, center, degree);

    float oldPheromone = 68.;
    float newPheromone = policy.evaporate(oldPheromone, 2*center);
    CHECK(newPheromone<=(oldPheromone-plateau));

}

TEST(CubicEvaporationPolicyTest, testEvaporateZeroTimeSinceTraffic) {
    //Test checks that after zero seconds since last traffic, pheromone is unchanged
    float plateau = 46.7;
    float center =  12.2;
    int degree = 13;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, center, degree);

    float oldPheromone = 99.;
    float newPheromone = policy.evaporate(oldPheromone, 0);
    DOUBLES_EQUAL(oldPheromone, newPheromone, 0);

}

TEST(CubicEvaporationPolicyTest, testEvaporateAtPlateauCenter){
    //Test checks that at desired plateau center evaporation is at desired plateau value
    float plateau = 47.2;
    float center =  63.2;
    int degree = 3;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, center, degree);

    float oldPheromone = 123.8;
    float newPheromone = policy.evaporate(oldPheromone, center);
    DOUBLES_EQUAL((oldPheromone-plateau), newPheromone, 0.00001);
}

TEST(CubicEvaporationPolicyTest, testEvaporateUpperPlateauCorner){
    //Test checks that pheromone is within precision of plateau near upper corner
    float plateau = 47.2;
    float center =  63.2;
    int degree = 3;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, center, degree);

    float oldPheromone = 123.8;
    float precision = 0.01;

    //Using Friedrich's equation 3.13, pow is used as root
    float t1 = center + pow((pow(center,degree)*precision/plateau),(1.0/degree));
    float newPheromone = policy.evaporate(oldPheromone, t1);

    //Phrased this way such that floating point values don't cause erroneous failure
    CHECK((newPheromone-(oldPheromone-plateau))- precision <= .0001);
}

TEST(CubicEvaporationPolicyTest, testEvaporateLowerPlateauCorner){
    //Test checks that pheromone is within precision of plateau near upper corner
    float plateau = 47.2;
    float center =  63.2;
    int degree = 3;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, center, degree);

    float oldPheromone = 123.8;
    float precision = 0.001;
    //Using Friedrich's equation 3.13; compiler can't handle negative odd roots, so moved negative
    //mathematically gives the negative real root without tripping compiler
    float t1 = center - pow((pow(center,degree)*precision/plateau),(1.0/degree));
    float newPheromone = policy.evaporate(oldPheromone, t1);
    DOUBLES_EQUAL((oldPheromone-plateau), newPheromone, precision);
}

TEST(CubicEvaporationPolicyTest, testLowerPheromoneBound){
    //Test checks that at desired plateau center evaporation is at desired plateau value
    float plateau = 47.2;
    float center =  63.2;
    int degree = 3;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, center, degree);

    float oldPheromone = 123.8;
    float newPheromone = policy.evaporate(oldPheromone, 345237567567654567);
    DOUBLES_EQUAL(0, newPheromone, 0.00001);
}
