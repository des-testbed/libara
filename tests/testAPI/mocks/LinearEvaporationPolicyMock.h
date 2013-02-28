/*
 * $FU-Copyright$
 */

#ifndef LINEAR_EVAPORATION_POLCY_MOCK_H_
#define LINEAR_EVAPORATION_POLCY_MOCK_H_

#include "EvaporationPolicy.h"

namespace ARA {
    class LinearEvaporationPolicyMock : public EvaporationPolicy {
        public:
            LinearEvaporationPolicyMock(float evaporationFactor=0.2, long intervallInMilliSeconds=10000);
            virtual float evaporate(float phi);
            float getEvaporationFactor() const;

        private:
            float evaporationFactor;
    };
}

#endif
