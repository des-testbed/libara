/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_
#define ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_

#include "ARAMacros.h"
#include "StochasticForwardingPolicy.h"
#include "EARAForwardingPolicy.h"
#include "EnergyAwareRoutingTable.h"
#include "NextHop.h"
#include "Packet.h"

ARA_NAMESPACE_BEGIN

/**
 * The class provides the forwarding policy for the energy aware ant routing (EARA).
 */
class EnergyAwareStochasticForwardingPolicy : public StochasticForwardingPolicy, public virtual EARAForwardingPolicy {
    public:
        EnergyAwareStochasticForwardingPolicy(EnergyAwareRoutingTable* routingTable, float pheromoneWeight = 1.0, float energyWeight = 2.0);
        virtual ~EnergyAwareStochasticForwardingPolicy(){};

        virtual NextHop* getNextHop(const Packet* packet);
        virtual float getPheromoneWeight();
        virtual float getEnergyWeight();

        void setPheromoneWeight(float alpha);
        void setEnergyWeight(float beta);

    private:
        int getRandomNodeIndex(float cumulativeSum[]);

    protected:
        EnergyAwareRoutingTable* routingTable;

        /** the weight of the pheromone variable in the transmission probability **/
        float pheromoneWeight;

        /** the weight of the energy variable in the transmission probability **/
        float energyWeight;
};

ARA_NAMESPACE_END

#endif
