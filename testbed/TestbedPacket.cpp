/*
 * $FU-Copyright$
 */

#include "TestbedPacket.h"

TESTBED_NAMESPACE_BEGIN

TestbedPacket::TestbedPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) : Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize) { 

}
  
AddressPtr TestbedPacket::getSource() const {
    std::lock_guard<std::mutex> lock(mutex);
    return Packet::getSource();
}

AddressPtr TestbedPacket::getDestination() const {
    std::lock_guard<std::mutex> lock(mutex);
    return Packet::getDestination();
}

AddressPtr TestbedPacket::getSender() const {
    std::lock_guard<std::mutex> lock(mutex);
    return Packet::getSender();
}

AddressPtr TestbedPacket::getPreviousHop() const {
    std::lock_guard<std::mutex> lock(mutex);
    return Packet::getPreviousHop();
}


TESTBED_NAMESPACE_END
