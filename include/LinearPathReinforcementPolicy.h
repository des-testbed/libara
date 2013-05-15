/*
 * $FU-Copyright$
 */

#ifndef LINEAR_PATH_REINFORCEMENT_POLICY_H_
#define LINEAR_PATH_REINFORCEMENT_POLICY_H_

#include "PathReinforcementPolicy.h"

namespace ARA {
    class LinearPathReinforcementPolicy : public PathReinforcementPolicy {
        public:
            LinearPathReinforcementPolicy(float deltaPhi);
            float calculateReinforcedValue(float oldPheromoneValue);

            void setDeltaPhi(float deltaPhi);
        private:
            float deltaPhi;
    };
}

#endif
