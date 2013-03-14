/*
 * $FU-Copyright$
 */

#ifndef PACKETTRAP_H_
#define PACKETTRAP_H_

#include "RoutingTable.h"
#include "Packet.h"
#include "Address.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <deque>

namespace ARA {

class PacketTrap {
public:
    PacketTrap(RoutingTable* routingTable);
    ~PacketTrap();

    /**
     * Stores the given packet in the packet trap until it is removed via
     * PacketTrap::untrapPacket(Packet*).
     *
     * All packets that are still trapped when the PacketTrap destructor is called are destroyed.
     */
    void trapPacket(Packet* packet);

    /**
     * Removes the given packet from this packet trap.
     */
    void untrapPacket(Packet* packet);

    bool contains(Packet* packet);
    bool isEmpty();

    /**
     * TODO maybe this should untrap the packets by default!
     * Returns a new list of packets that are deliverable according to the
     * routing table associated with this packet trap.
     *
     * Note: The LinkedList must be deleted by the caller of this method
     */
    std::deque<Packet*>* getDeliverablePackets();

    /**
     * This will remove all packets for the given destination address from this packet trap
     * and return them in a list object.
     */
    std::deque<Packet*> removePacketsForDestination(std::shared_ptr<Address> destination);

    void setRoutingTable(RoutingTable *routingTable);

private:

    bool thereIsAHashSetFor(std::shared_ptr<Address> destination);

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
    std::unordered_map<std::shared_ptr<Address>, std::unordered_set<Packet*, PacketHash, PacketPredicate>*, AddressHash, AddressPredicate> trappedPackets;

    RoutingTable* routingTable;

};

} /* namespace ARA */
#endif /* PACKETTRAP_H_ */
