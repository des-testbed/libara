/*
 * $FU-Copyright$
 */

#ifndef OMNETGATE_H_
#define OMNETGATE_H_

#include "AbstractNetworkInterface.h"
#include "ARA.h"
#include "OMNeTAddress.h"
#include "InterfaceEntry.h"

#include <omnetpp.h>
#include <memory>

namespace ARA {
namespace omnetpp {

class ARA;

//TODO write some more documentation for this class
    class OMNeTGate: public AbstractNetworkInterface {
    public:
        OMNeTGate(ARA* module, cGate* gate, InterfaceEntry* interfaceEntry, double broadCastDelay, double uniCastDelay);

        void send(const Packet* packet, std::shared_ptr<Address> recipient);
        void send(const Packet* packet, std::shared_ptr<Address> recipient, double sendDelay);

        void broadcast(const Packet* packet);
        bool equals(NetworkInterface* interface);

    private:
        std::shared_ptr<OMNeTAddress> getNextHopAddress(std::shared_ptr<Address> recipient);

    private:
        ARA* omnetARAClient;
        cGate* gateToARP;
        int interfaceID;

        /**
         * The delay in seconds that is added to broadcast operations to
         * prevent packet collision by perfect synchronization in the simulation
         */
        double broadCastDelay;

        /**
         * The delay in seconds that is added to unicast messaged.
         * It is used to model processing time and prevents perfect
         * event synchronization which would lead to packet collisions
         */
        double uniCastDelay;
    };

} /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNETGATE_H_ */
