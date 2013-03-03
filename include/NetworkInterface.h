/*
 * $FU-Copyright$
 */

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include <memory>
#include "Packet.h"
#include "Address.h"

namespace ARA {

/**
 * Network interface are used by the ARA clients to send Packets to other clients.
 */
class NetworkInterface {
public:
    virtual ~NetworkInterface() {}
    
    /**
     * Sends the packet over this interface to the given recipient.
     */
	virtual void send(const Packet* packet, std::shared_ptr<Address> recipient) = 0;

	/**
	 * Broadcasts the given packet over this interface.
	 *
	 * Note: After this method returns, the packet object may be deleted at
	 * any point so if this NetworkInterface needs access to this object any
	 * longer it needs to clone the packet itself.
	 */
	virtual void broadcast(const Packet* packet) = 0;

	/**
	 * Compares this interface to another interface.
	 *
	 * This is needed to find the right interface to send over if an ARA client
	 * has more than one registered network interface.
	 */
	virtual bool equals(NetworkInterface* interface) = 0;

	/**
	 * Returns the address over which this network interface is reachable by other clients.
	 *
	 * This method is used to determine the sender address each time a a new packet is created.
	 */
	virtual std::shared_ptr<Address> getLocalAddress() = 0;

	/**
	 * Returns true if the given address is known to be a broadcast address.
	 * This method is now implemented at the network interface because this is
	 * the only object that is officially required to recognize a broadcast
	 * address.
	 *
	 * Example: An ordinary IPv4 Address object by itself may not be able to
	 * distinguish between a normal address and a broadcast address without
	 * knowing its subnet mask.
	 */
	virtual bool isBroadcastAddress(std::shared_ptr<Address> someAddress) const = 0;
};

} /* namespace ARA */
#endif // NETWORKINTERFACE_H_
