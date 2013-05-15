/*
 * $FU-Copyright$
 */

#include "LinearPathReinforcementPolicy.h"

using namespace ARA;

LinearPathReinforcementPolicy::LinearPathReinforcementPolicy(float deltaPhi) {
    this->deltaPhi = deltaPhi;
}

float LinearPathReinforcementPolicy::calculateReinforcedValue(float oldPheromoneValue) {
     return oldPheromoneValue + deltaPhi;
}

void LinearPathReinforcementPolicy::setDeltaPhi(float deltaPhi) {
    this->deltaPhi = deltaPhi;
}
