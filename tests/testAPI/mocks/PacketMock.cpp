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
 : Packet(AddressPtr(new AddressMock(sourceName)), AddressPtr(new AddressMock(destinationName)), AddressPtr(new AddressMock(sourceName)), packetType, sequenceNumber, nullptr, 0, hopCount){
    const char* payloadString = "Hello World";
    payloadSize = std::strlen(payloadString);
    char* tmpPayload = new char[payloadSize+1];
    strncpy(tmpPayload, payloadString, payloadSize);
    tmpPayload[payloadSize] = '\0';
    payloadSize++;
    payload = tmpPayload;
}

PacketMock::PacketMock(const char* sourceName, const char* destinationName, const char* senderName, unsigned int sequenceNumber, unsigned int hopCount, char packetType, const char* payloadString)
 : Packet(AddressPtr(new AddressMock(sourceName)), AddressPtr(new AddressMock(destinationName)), AddressPtr(new AddressMock(senderName)), packetType, sequenceNumber, payloadString, std::strlen(payloadString)+1, hopCount){
}

void PacketMock::setSender(std::shared_ptr<Address> newSenderAddress) {
    sender = newSenderAddress;
}

} /* namespace ARA */
