/*
 * $FU-Copyright$
 */

#ifndef OMNET_TIMEFACTORY_H_
#define OMNET_TIMEFACTORY_H_

#include "TimeFactory.h"

namespace ARA {
namespace omnetpp {

    class OMNeTTimeFactory : public TimeFactory {
        public:
            Time* makeTime();
    };
}
}
#endif
