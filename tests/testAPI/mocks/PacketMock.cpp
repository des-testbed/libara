/*
 * $FU-Copyright$
 */

#include "PacketMock.h"
#include "PacketType.h"
#include "AddressMock.h"
#include <sstream>
#include <cstring>

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

PacketMock::PacketMock(const char* sourceName, const char* destinationName, unsigned int sequenceNumber, unsigned int hopCount, char packetType)
 : Packet(AddressPtr(new AddressMock(sourceName)), AddressPtr(new AddressMock(destinationName)), AddressPtr(new AddressMock(sourceName)), packetType, sequenceNumber, "Hello World", 11, hopCount){
}

PacketMock::PacketMock(const char* sourceName, const char* destinationName, const char* senderName, unsigned int sequenceNumber, unsigned int hopCount, char packetType, const char* payload)
 : Packet(AddressPtr(new AddressMock(sourceName)), AddressPtr(new AddressMock(destinationName)), AddressPtr(new AddressMock(senderName)), packetType, sequenceNumber, payload, std::strlen(payload), hopCount){
}

void PacketMock::setSender(std::shared_ptr<Address> newSenderAddress) {
    sender = newSenderAddress;
}

} /* namespace ARA */
