/*
 * $FU-Copyright$
 */

#ifndef OMNETARACLIENT_H_
#define OMNETARACLIENT_H_

#include <omnetpp.h>

#include "AbstractARAClient.h"
#include "Packet.h"
#include "OMNeTGate.h"
#include "OMNeTConfiguration.h"
#include "MessageDispatcher.h"
#include "NetworkInterface.h"
#include "IInterfaceTable.h"


namespace ARA {
    namespace omnetpp {

        class MessageDispatcher;

        /**
         * The class represents the implementation of the ant routing algorithm (ARA)
         * for the OMNeT++ simulation framework.
         *
         * The algorithm was first published in:
         *
         *  Mesut Guenes, Udo Sorges, and Imed Bouazizi. "ARA-the ant-colony based routing algorithm for MANETs."
         *  Parallel Processing Workshops, 2002. Proceedings. International Conference on. IEEE, 2002.
         */
        class OMNeTARA: public cSimpleModule, public AbstractARAClient {
            public:
                OMNeTARA();
                ~OMNeTARA();

                void handleRouteFailure(const Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

            protected:
                int numInitStages() const;
                virtual void initialize(int stage);
                virtual void handleMessage(cMessage *msg);

                void deliverToSystem(const Packet* packet);
                void packetNotDeliverable(const Packet* packet);

                /**
                 * Method for friend class OMNeTGate.
                 * It switches the context to the ARAClient,
                 * takes ownership of the packet and sends it to the specified gate
                 * with the given delay.
                 */
                void takeAndSend(cMessage* msg, cGate* gate, double sendDelay = 0);
            private:
                MessageDispatcher* messageDispatcher;
                IInterfaceTable* interfaceTable;

            friend class OMNeTGate;
            friend class OMNeTConfiguration;
            friend class MessageDispatcher;
        };

    } /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNETARACLIENT_H_ */
