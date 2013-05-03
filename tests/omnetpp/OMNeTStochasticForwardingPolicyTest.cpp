/*
 * $FU-Copyright$
 */

#include <memory>
#include <iostream>
#include "CppUTest/TestHarness.h"

#include "testAPI/mocks/AddressMock.h"
#include "testAPI/mocks/PacketMock.h"

#include "OMNeTStochasticForwardingPolicy.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(OMNeTStochasticForwardingPolicyTest) {};

// can't test the method since it is protected and the unit test is not a 'friend'
/*
TEST(OMNeTStochasticForwardingPolicyTest, testGetRandomNumber) {
    // create a routing table without content 
    RoutingTable routingTable = RoutingTable();
    // create a policy
    OMNeTStochasticForwardingPolicy policy(&routingTable);
    // try to get a random number
    float randomNumber = policy.getRandomNumber();

    std::cout << " random number is " << randomNumber << std::endl;
}
*/
