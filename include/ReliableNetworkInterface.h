/*
 * $FU-Copyright$
 */

#ifndef RELIABLE_NETWORK_INTERFACE_H_
#define RELIABLE_NETWORK_INTERFACE_H_

#include "AbstractNetworkInterface.h"
#include "AbstractARAClient.h"

#include <memory>

namespace ARA {

    /**
     * TODO write class description
     */
    class ReliableNetworkInterface : public AbstractNetworkInterface {
        public:
            /**
             * Creates a new ReliableNetworkInterface.
             *
             * @param client the AbstractARAClient to which all received packets are route and which is informed if a packet is not deliverable.
             * @param localAddress the Address which is returned when the interface is asked for its local address.
             * @param broadcastAddress the Address which is known to be the broadcast address of this interface.
             */
            ReliableNetworkInterface(AbstractARAClient* client, std::shared_ptr<Address> localAddress = nullptr, std::shared_ptr<Address> broadcastAddress = nullptr);
            virtual ~ReliableNetworkInterface();

            /**
             * Implements a reliable send operation which will deliver the packet to the given recipient.
             * To accomplish this the interface waits for a packet acknowledgment and sends the packet again
             * if it is not acknowledged in a given time interval.
             */
            void send(const Packet* packet, std::shared_ptr<Address> recipient);

            std::deque<const Packet*> getUnacknowledgedPackets() const;

        protected:

            /**
             * This method needs to be implemented by the concrete ReliableNetworkInterface implementation
             * It must send the given packet to the wanted recipient via this interface.
             */
            virtual void doSend(const Packet* packet, std::shared_ptr<Address> recipient) = 0;

        private:
            std::deque<const Packet*> unacknowledgedPackets;
    };

} /* namespace ARA */
#endif // ABSTRACT_NETWORK_INTERFACE_H_
