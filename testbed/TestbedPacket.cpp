/*
 * $FU-Copyright$
 */

#include "TestbedPacket.h"

TESTBED_NAMESPACE_BEGIN

TestbedPacket::TestbedPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) 
  : Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize) { }


TestbedPacket::TestbedPacket(const TestbedPacket& packet) {
    /** 
     * For every dynamic_pointer_cast operation we check if the operation was
     * successfull by checking if the (resulting) shared_ptr is set.
     */
    std::shared_ptr<TestbedAddress> address = std::dynamic_pointer_cast<TestbedAddress>(packet.getSource());
    if (address) {
        source = std::make_shared<TestbedAddress>(address->getDessertValue());
    }

    address = std::dynamic_pointer_cast<TestbedAddress>(packet.getDestination());
    if (address) {
        destination = std::make_shared<TestbedAddress>(address->getDessertValue());
    }

    address = std::dynamic_pointer_cast<TestbedAddress>(packet.getSender());
    if (address) {
        sender = std::make_shared<TestbedAddress>(address->getDessertValue());
    }

    address = std::dynamic_pointer_cast<TestbedAddress>(packet.getPreviousHop());
    if (address) {
        previousHop = std::make_shared<TestbedAddress>(address->getDessertValue());
    }

    ttl = packet.getTTL();
    type = packet.getType();
    seqNr = packet.getSequenceNumber();

    payloadSize = packet.getPayloadLength();
    
    char* temporaryPayload = new char[payloadSize];
    std::copy(packet.getPayload(), packet.getPayload() + packet.getPayloadLength(), temporaryPayload);
    payload = temporaryPayload;
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
