/*
 * $FU-Copyright$
 */

#ifndef EXPONENTIAL_EVAPORATION_POLCY_MOCK_H_
#define EXPONENTIAL_EVAPORATION_POLCY_MOCK_H_

#include "ExponentialEvaporationPolicy.h"

namespace ARA {
    class ExponentialEvaporationPolicyMock : public ExponentialEvaporationPolicy {
        public:
            ExponentialEvaporationPolicyMock(float evaporationFactor = 0.8, int timeIntervalMillis = 1000, float threshold = 0.5);
            float getEvaporationFactor() const;
    };
}

#endif
