/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_NETWORK_INTERFACE_H_
#define ABSTRACT_NETWORK_INTERFACE_H_

#include "NetworkInterface.h"
#include "AbstractARAClient.h"

#include <memory>

namespace ARA {

    /**
     * TODO write class description
     */
    class AbstractNetworkInterface : public NetworkInterface {
        public:
            /**
             * Creates a new AbstractNetworkInterface.
             *
             * @param client the AbstractARAClient to which all received packets are route and which is informed if a packet is not deliverable.
             * @param localAddress the Address which is returned when the interface is asked for its local address.
             * @param broadcastAddress the Address which is known to be the broadcast address of this interface.
             */
            AbstractNetworkInterface(AbstractARAClient* client, std::shared_ptr<Address> localAddress = nullptr, std::shared_ptr<Address> broadcastAddress = nullptr);
            virtual ~AbstractNetworkInterface() {}

            std::shared_ptr<Address> getLocalAddress() const;
            bool isBroadcastAddress(std::shared_ptr<Address> someAddress) const;

            /**
             * Receive a packet over this interface.
             * A received packet is normally delivered to the ARAClient but
             * this might not be the case with some special packets packets
             * like acknowledgments or hello packets.
             *
             * However the standard implementation just sends everything directly to the client.
             * To change that behavior overwrite this method
             *
             * @see AbstractARAClient::receivePacket(Packet* packet, NetworkInterface* interface)
             */
            virtual void receive(Packet* packet);

        protected:
            /**
             * This method can be used by the concrete implementations of this class.
             * It will send the given packet to the ARAClient that is associated to this
             * interface.
             */
            void deliverToARAClient(Packet* packet);

        protected:
            AbstractARAClient* client;
            std::shared_ptr<Address> localAddress;
            std::shared_ptr<Address> broadcastAddress;
    };

} /* namespace ARA */
#endif // ABSTRACT_NETWORK_INTERFACE_H_
