/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "CubicEvaporationPolicy.h"

using namespace ARA;

TEST_GROUP(CubicEvaporationPolicyTest) {};

IGNORE_TEST(CubicEvaporationPolicyTest, testEvaporate) {
    float plateau = 3;
    float slow = 0.3;
    float reduction = 900.;
    float threshold = 0.2;
    unsigned int timeInterval = 200;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, slow, reduction, threshold, timeInterval);

    float pheromone = 1.;

    pheromone = policy.evaporate(pheromone, timeInterval);
    DOUBLES_EQUAL(0.966666, pheromone, 0.00001);

    
/*
    /// sleep for 10 ms; usleep not recognized by compiler
    //usleep(200);
    // check the status; also not recognized
    //status = policy.checkForEvaporation();
    // the 'last access time' should have been intialized; unrecognized
    //CHECK(status);
    // simply test the evaporate function; requires another argument
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

TEST(CubicEvaporationPolicyTest, evaporateWithZeroTimeInterval) {
    float plateau = 3;
    float slow = 0.2;
    float reduction = 0.3;
    float threshold = 0.2;
    unsigned int timeInterval = 200;
    CubicEvaporationPolicy policy = CubicEvaporationPolicy(plateau, slow, reduction, threshold, timeInterval);

    float pheromone = 10;

    pheromone = policy.evaporate(pheromone, 0);
    DOUBLES_EQUAL(10, pheromone, 0.00001);
}
