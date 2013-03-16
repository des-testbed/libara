/*
 * $FU-Copyright$
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "EvaporationPolicy.h"
#include "PathReinforcementPolicy.h"
#include "ForwardingPolicy.h"

namespace ARA {

    class Configuration {
    public:
        Configuration(EvaporationPolicy* evaporationPolicy, PathReinforcementPolicy* reinforcementPolicy, ForwardingPolicy* forwardingPolicy);

        EvaporationPolicy* getEvaporationPolicy();
        PathReinforcementPolicy* getReinforcementPolicy();
        ForwardingPolicy* getForwardingPolicy();

    private:
        EvaporationPolicy* evaporationPolicy;
        PathReinforcementPolicy* reinforcementPolicy;
        ForwardingPolicy* forwardingPolicy;
    };

} /* namespace ARA */
#endif /* CONFIGURATION_H_ */
