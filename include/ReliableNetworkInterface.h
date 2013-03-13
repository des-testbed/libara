/*
 * $FU-Copyright$
 */

#ifndef RELIABLE_NETWORK_INTERFACE_H_
#define RELIABLE_NETWORK_INTERFACE_H_

#include "AbstractNetworkInterface.h"
#include "TimeoutEventListener.h"
#include "AbstractARAClient.h"

#include <memory>

namespace ARA {

    /**
     * TODO write class description
     */
    class ReliableNetworkInterface : public AbstractNetworkInterface, public TimeoutEventListener {

        struct AckTimerData {
            int nrOfRetries;
            const Packet* packet;
            std::shared_ptr<Address> recipient;
        };

        public:
            /**
             * Creates a new ReliableNetworkInterface.
             *
             * @param client the AbstractARAClient to which all received packets are route and which is informed if a packet is not deliverable.
             * @param localAddress the Address which is returned when the interface is asked for its local address.
             * @param broadcastAddress the Address which is known to be the broadcast address of this interface.
             */
            ReliableNetworkInterface(AbstractARAClient* client, int ackTimeoutInMicroSeconds, std::shared_ptr<Address> localAddress=nullptr, std::shared_ptr<Address> broadcastAddress=nullptr);
            virtual ~ReliableNetworkInterface();

            /**
             * Implements a reliable send operation which will deliver the packet to the given recipient.
             * To accomplish this the interface waits for a packet acknowledgment and sends the packet again
             * if it is not acknowledged in a given time interval.
             *
             * The Packet will be deleted as soon as the packet has been acknowledged by the client or
             * is reported undeliverable to the associated ARA client.
             */
            void send(const Packet* packet, std::shared_ptr<Address> recipient);

            /**
             * Broadcasts the packet using the broadcast address that was provided in the class
             * constructor. Internally the doSend Method is used.
             */
            void broadcast(const Packet* packet);

            /**
             * Receive a packet over this interface. If this is a DATA packet it will be
             * acknowledged to the sender. If this is an ACK packet it will be used to
             * remove packets from the list of unacknowledged packets (and stop their timer).
             * If this is not an ACK packet it will be delivered to the associated AbstractARAClient.
             */
            virtual void receive(Packet* packet);

            std::deque<const Packet*> getUnacknowledgedPackets() const;

            void setMaxNrOfRetransmissions(int n);

            void timerHasExpired(Timer* responsibleTimer);

        protected:

            /**
             * This method needs to be implemented by the concrete ReliableNetworkInterface implementation
             * It must send the given packet to the wanted recipient via this interface.
             *
             * The packet is deleted by the ReliableNetworkInterface class and might cease
             * to exist as soon as this method returns.
             */
            virtual void doSend(const Packet* packet, std::shared_ptr<Address> recipient) = 0;


        protected:
            std::deque<const Packet*> unacknowledgedPackets;
            std::unordered_map<Timer*, AckTimerData> runningTimers;
            double ackTimeoutInMicroSeconds;
            int maxNrOfRetransmissions = 5;

        private:
            void startAcknowledgmentTimer(const Packet* packet, std::shared_ptr<Address> recipient);
            void handleUndeliverablePacket(Timer* ackTimer, AckTimerData& timerData);
            void handleNonAckPacket(Packet* packet);
            void handleAckPacket(Packet* packet);
    };

} /* namespace ARA */
#endif // ABSTRACT_NETWORK_INTERFACE_H_
