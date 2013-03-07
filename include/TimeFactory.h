/*
 * $FU-Copyright$
 */

#ifndef TIMEFACTORY_H_
#define TIMEFACTORY_H_

#include "Time.h"
#include "Timer.h"

namespace ARA {

    /**
     * A TimeFactory is responsible for instantiating a concrete instance of
     * the abstract Time class. For each concrete implementation of Time there
     * will be one TimeFactory counterpart that will handle instantiation in
     * the EvaporationPolicy class.
     */
    class TimeFactory { //TODO maybe we should rename TimeFactory to Clock to make its intend more clear
        public:
            virtual ~TimeFactory() {};

            /**
             * Creates a new Time instance. The pointer must be deleted by the
             * invoking object.
             */
            virtual Time* makeTime() = 0;

            /**
             * Creates a new Timer instance. The pointer must be deleted by the
             * invoking object.
             */
            virtual Timer* getNewTimer() = 0;
    };
}

#endif
