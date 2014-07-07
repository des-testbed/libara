/*
 * $FU-Copyright$
 */

#ifndef PACKETTRAP_H_
#define PACKETTRAP_H_

#include "ARAMacros.h"
#include "Packet.h"
#include "RoutingTable.h"

#include <unordered_map>
#include <deque>

ARA_NAMESPACE_BEGIN

typedef std::deque<Packet*> PacketQueue;
typedef std::unordered_map<AddressPtr, PacketQueue, AddressHash, AddressPredicate> TrappedPacketsMap;

/**
 * The PacketTrap is responsible for storing packets while the route discovery
 * is running for their respective destinations.
 */
class PacketTrap {
public:

    /**
     * Creates a new PacketTrap which uses the given routingTable to check if
     * packets are deliverable or not.
     */
    PacketTrap(RoutingTable* routingTable);

    /**
     * Deletes the PacketTrap and especially all packets that are still trapped.
     */
    virtual ~PacketTrap();

    /**
     * Stores the given packet in the packet trap until it is removed via
     * PacketTrap::untrapDeliverablePackets(..). The order of trapped packets
     * is preserved in a FIFO style.
     *
     * All packets that are still trapped when the PacketTrap destructor is called are deleted.
     */
    virtual void trapPacket(Packet* packet);

    /**
     * Returns true if this packet trap contains a given packet.
     * False otherwise.
     */
    virtual bool contains(Packet* packet);

    /**
     * Returns true if the number of trapped packets equals zero.
     * False otherwise.
     */
    virtual bool isEmpty();

    /**
     * Returns a new list of packets that are deliverable to a given destination
     * according to the routing table associated with this packet trap.
     * The packets are in the same order as PacketTrap::trapPacket(..) has been
     * called on them. All returned packets are automatically untrapped.
     */
    virtual PacketQueue untrapDeliverablePackets(AddressPtr destination);

    /**
     * This will remove all packets for the given destination address from this packet trap
     * and return them in a list object.
     */
    virtual PacketQueue removePacketsForDestination(AddressPtr destination);

    /**
     * Returns the number of trapped packets for a given destination or the
     * total number of all trapped packets if destination is the nullptr.
     * This will most likely only be used for statistics and performance analysis.
     */
    virtual unsigned int getNumberOfTrappedPackets(AddressPtr destination=nullptr);

    /**
     * Set the assigned routing table.
     * It is necessary to implement this function because some ARA implementations
     * might need to change their routing tables at runtime (e.g. OMNeT++)
     */
    virtual void setRoutingTable(RoutingTable* routingTable) {
        this->routingTable = routingTable;
    }

private:

    /**
     * This hashmap stores all trapped packets.
     * In Java we would write: HashMap<Address, Queue<Packet>>
     * The keys are the packet destinations and the values are a
     * queue of packets.
     *
     * This is a Hashmap because everytime PacketTrap::getDeliverablePackets()
     * is called we want to find all packets for a specific destination fast.
     * The Values are stored as queue to keep the order of sent packets.
     */
    TrappedPacketsMap trappedPackets;

    /**
     * The associated routing table.
     * Please not that this is managed by the ARAClient and must not be deleted
     * by the packet trap.
     */
    RoutingTable* routingTable;
};

ARA_NAMESPACE_END

#endif
