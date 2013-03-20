/*
 * $FU-Copyright$
 */

#ifndef OMNETARACLIENT_H_
#define OMNETARACLIENT_H_

#include <omnetpp.h>

#include "AbstractARAClient.h"
#include "Packet.h"
#include "Energy.h"
#include "OMNeTGate.h"
#include "OMNeTConfiguration.h"
#include "MessageDispatcher.h"
#include "NetworkInterface.h"
#include "IInterfaceTable.h"
#include "INotifiable.h"
#include "ARP.h"

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
        class OMNeTARA: public cSimpleModule, public AbstractARAClient, public INotifiable {
            public:
                OMNeTARA();
                ~OMNeTARA();

            protected:
                int numInitStages() const;
                virtual void initialize(int stage);
                virtual void handleMessage(cMessage *msg);

                /**
                 * Called by the NotificationBoard whenever a change of a category
                 * occurs to which this client has subscribed.
                 */
                void receiveChangeNotification(int category, const cObject* details);

                /**
                 * The packet should be directed to this node and must be delivered to the local system.
                 * Please note that this method is responsible for deleting the given packet (or delegating
                 * this responsibility to another method)
                 */
                void deliverToSystem(const Packet* packet);

                /**
                 * This method is called if the route discovery is unsuccessful and not route to the packets
                 * destination can be established. The task of this method is to notify the upper layers
                 * about this event and delete the packet.
                 */
                void packetNotDeliverable(const Packet* packet);

            private:
                /**
                 * Method for friend class OMNeTGate.
                 * It switches the context to the ARAClient,
                 * takes ownership of the packet and sends it to the specified gate
                 * with the given delay.
                 */
                void takeAndSend(cMessage* msg, cGate* gate, double sendDelay = 0);

                void initializeNetworkInterfacesOf(OMNeTConfiguration& config);

                MessageDispatcher* messageDispatcher;
                IInterfaceTable* interfaceTable;
                ARP* arp;

            friend class OMNeTGate;
            friend class OMNeTConfiguration;
            friend class MessageDispatcher;
        };

    } /* namespace ARA */
} /* namespace omnetpp */

#endif /* OMNETARACLIENT_H_ */
