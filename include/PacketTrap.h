/*
 * $FU-Copyright$
 */

#ifndef PACKETTRAP_H_
#define PACKETTRAP_H_

#include "ARAMacros.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "Address.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <deque>

ARA_NAMESPACE_BEGIN

typedef std::unordered_map<AddressPtr, std::unordered_set<Packet*, PacketHash, PacketPredicate>*, AddressHash, AddressPredicate> TrappedPacketsMap;

class PacketTrap {
public:
    PacketTrap(RoutingTable* routingTable);
    ~PacketTrap();

    /**
     * Stores the given packet in the packet trap until it is removed via
     * PacketTrap::untrapDeliverablePackets(..).
     *
     * All packets that are still trapped when the PacketTrap destructor is called are deleted.
     */
    void trapPacket(Packet* packet);

    /**
     * Returns true if this packet trap contains a given packet.
     * False otherwise.
     */
    bool contains(Packet* packet);

    /**
     * Returns true if the number of trapped packets equals zero.
     * False otherwise.
     */
    bool isEmpty();

    /**
     * Returns a new list of packets that are deliverable to a given destination
     * according to the routing table associated with this packet trap.
     * All returned packets are automatically untrapped.
     *
     * Note: The LinkedList must be deleted by the caller of this method
     */
    std::deque<Packet*>* untrapDeliverablePackets(AddressPtr destination);

    /**
     * This will remove all packets for the given destination address from this packet trap
     * and return them in a list object.
     */
    std::deque<Packet*> removePacketsForDestination(AddressPtr destination);

    /**
     * Returns the number of trapped packets for a given destination or the
     * total number of all trapped packets if destination is the nullptr.
     * This will most likely only be used for statistics and performance analysis.
     */
    unsigned int getNumberOfTrappedPackets(AddressPtr destination=nullptr);

    //TODO maybe remove this? do we really need to set the table dynamically?
    void setRoutingTable(RoutingTable *routingTable);

private:

    bool thereIsAHashSetFor(AddressPtr destination);

    /**
     * This hashmap stores all trapped packets.
     * In Java we would write: HashMap<Address, HashSet<Packet>>
     * The keys are the packet destinations and the values are a
     * set of packets.
     *
     * This is a Hashmap because everytime PacketTrap::getDeliverablePackets()
     * is called we want to find all packets for a specific destination fast.
     * The Values are hashsets themselves because we also have to find individual
     * packets everytime we want to untrap a packet (i.e. after acknowledgment).
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
