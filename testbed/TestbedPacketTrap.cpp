/*
 * $FU-Copyright$
 */

#include "TestbedPacketTrap.h"

TESTBED_NAMESPACE_BEGIN

TestbedPacketTrap::TestbedPacketTrap(RoutingTable *routingTable) : PacketTrap(routingTable) { }

void TestbedPacketTrap::setRoutingTable(RoutingTable* routingTable) {
    assert(routingTable != nullptr);
    std::lock_guard<std::mutex> lock(routingTableMutex);
    PacketTrap::setRoutingTable(routingTable);
}

void TestbedPacketTrap::trapPacket(Packet* packet) {
    assert(packet != nullptr);
    std::lock_guard<std::mutex> lock(packetTrapMutex);
    PacketTrap::trapPacket(packet);
}

bool TestbedPacketTrap::isEmpty() {
    std::lock_guard<std::mutex> lock(packetTrapMutex);
    bool result = PacketTrap::isEmpty();
    return result;
}

bool TestbedPacketTrap::contains(Packet *packet) {
    std::lock_guard<std::mutex> lock(packetTrapMutex);
    bool result = PacketTrap::contains(packet);
    return result;
}

unsigned int TestbedPacketTrap::getNumberOfTrappedPackets(AddressPtr destination) {
    std::lock_guard<std::mutex> lock(packetTrapMutex);
    unsigned int result = PacketTrap::getNumberOfTrappedPackets(destination);
    return result;
}

PacketQueue TestbedPacketTrap::untrapDeliverablePackets(AddressPtr destination) {
    std::lock_guard<std::mutex> lock(packetTrapMutex);
    PacketQueue result = PacketTrap::untrapDeliverablePackets(destination);
    return result;
}

PacketQueue TestbedPacketTrap::removePacketsForDestination(AddressPtr destination) {
    std::lock_guard<std::mutex> lock(packetTrapMutex);
    PacketQueue result = PacketTrap::removePacketsForDestination(destination);
    return result;
}

TESTBED_NAMESPACE_END
