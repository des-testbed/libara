/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "LinearPathReinforcementPolicy.h"

using namespace ARA;

TEST_GROUP(LinearPathReinforcementPolicyTest) {};

TEST(LinearPathReinforcementPolicyTest, pathReinforcementViaRoutingTableUpdate) {
    float currentPhi = 5.1;
    float deltaPhi = 1.2;
    
    LinearPathReinforcementPolicy reinforcementPolicy = LinearPathReinforcementPolicy(deltaPhi);
    DOUBLES_EQUAL(currentPhi + deltaPhi, reinforcementPolicy.calculateReinforcedValue(currentPhi), 0.000001);
}
