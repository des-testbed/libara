/*
 * $FU-Copyright$
 */

#ifndef TESTBED_PACKETTRAP_H_
#define TESTBED_PACKETTRAP_H_

#include "Testbed.h"
#include "PacketTrap.h"
#include "RoutingTable.h"

#include <mutex>
#include <cassert>

TESTBED_NAMESPACE_BEGIN

/**
 * The PacketTrap is responsible for storing packets while the route discovery
 * is running for their respective destinations.
 */
class TestbedPacketTrap : public PacketTrap {
    public:
        TestbedPacketTrap(RoutingTable* routingTable);
        ~TestbedPacketTrap(){};

        /**
         * @see PacketTrap::trapPacket(Packet* packet)
         */
        void trapPacket(Packet* packet);

        /**
         * @see PacketTrap::contains(Packet* packet)
         */
        bool contains(Packet* packet);

        /**
         * @see PacketTrap::isEmpty()
         */
        bool isEmpty();

        /**
         * @see PacketTrap::untrapDeliverablePackets(AddressPtr destination)
         */
        PacketQueue untrapDeliverablePackets(AddressPtr destination);

        /**
         * @see PacketTrap::removePacketsForDestination(AddressPtr destination)
         */
        PacketQueue removePacketsForDestination(AddressPtr destination);

        /**
         * @see PacketTrap::getNumberOfTrappedPackets(AddressPtr
         * destination=nullptr)
         */
        unsigned int getNumberOfTrappedPackets(AddressPtr destination=nullptr);

        /**
         * @see PacketTrap::setRoutingTable(RoutingTable* routingTable)
         */
        void setRoutingTable(RoutingTable* routingTable);

    private:
        std::mutex packetTrapMutex;
        std::mutex routingTableMutex;
};

TESTBED_NAMESPACE_END

#endif
