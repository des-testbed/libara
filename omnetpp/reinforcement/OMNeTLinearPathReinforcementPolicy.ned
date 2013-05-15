@namespace(ARA::omnetpp);
package ara.reinforcement;

import ara.reinforcement.ReinforcementPolicy;

simple OMNeTLinearPathReinforcementPolicy like ReinforcementPolicy
{
    parameters:
        double deltaPhi;
}
