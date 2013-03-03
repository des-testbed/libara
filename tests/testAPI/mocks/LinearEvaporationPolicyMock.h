/*
 * $FU-Copyright$
 */

#ifndef LINEAR_EVAPORATION_POLCY_MOCK_H_
#define LINEAR_EVAPORATION_POLCY_MOCK_H_

#include "LinearEvaporationPolicy.h"

namespace ARA {
    class LinearEvaporationPolicyMock : public LinearEvaporationPolicy {
        public:
            LinearEvaporationPolicyMock(float evaporationFactor = 0.8, float threshold = 0.5, int timeIntervalMillis = 1000);
            float getEvaporationFactor() const;
    };
}

#endif
