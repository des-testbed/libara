/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_NETWORK_INTERFACE_H_
#define ABSTRACT_NETWORK_INTERFACE_H_

#include "ARAMacros.h"
#include "NetworkInterface.h"
#include "AbstractARAClient.h"

ARA_NAMESPACE_BEGIN

/**
 * This class provides some basic functionality and does already implement some of the
 * simple function from NetworkInterface like checking if an address is local or
 * broadcast. A broadcast is also implemented using the send method and a broadcast address
 * which has to be provided in the constructor.
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
        //TODO replace the first parameter by an interface "Receiver" (this is the only functionality this class requires)
        AbstractNetworkInterface(AbstractARAClient* client, AddressPtr localAddress = nullptr, AddressPtr broadcastAddress = nullptr);
        virtual ~AbstractNetworkInterface() {}

        AddressPtr getLocalAddress() const;
        bool isBroadcastAddress(AddressPtr someAddress) const;
        bool isLocalAddress(AddressPtr someAddress) const;

        /**
         * Sends the packet with the stored broadcast address as receiver.
         */
        void broadcast(const Packet* packet);

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
        AddressPtr localAddress;
        AddressPtr broadcastAddress;
};

ARA_NAMESPACE_END

#endif // ABSTRACT_NETWORK_INTERFACE_H_
