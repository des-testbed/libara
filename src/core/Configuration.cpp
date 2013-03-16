/*
 * $FU-Copyright$
 */

#include "Configuration.h"

namespace ARA {

Configuration::Configuration(EvaporationPolicy* evaporationPolicy, PathReinforcementPolicy* reinforcementPolicy, ForwardingPolicy* forwardingPolicy) {
    this->evaporationPolicy = evaporationPolicy;
    this->reinforcementPolicy = reinforcementPolicy;
    this->forwardingPolicy = forwardingPolicy;
}

EvaporationPolicy* Configuration::getEvaporationPolicy() {
    return evaporationPolicy;
}

PathReinforcementPolicy* Configuration::getReinforcementPolicy() {
    return reinforcementPolicy;
}

ForwardingPolicy* Configuration::getForwardingPolicy() {
    return forwardingPolicy;
}

} /* namespace ARA */
