/*
 * $FU-Copyright$
 */

#ifndef PATH_REINFORCEMENT_POLICY_H_
#define PATH_REINFORCEMENT_POLICY_H_

namespace ARA {

    /**
     * The class encapsulates how the pheromone values are reinforced when a client sends
     * packets over the links.
     */
    class PathReinforcementPolicy {
         public:
             virtual ~PathReinforcementPolicy() {};

             virtual float calculateReinforcedValue(float oldPheromoneValue) = 0;
    };
}

#endif
