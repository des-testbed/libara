/*
 * $FU-Copyright$
 */

#include "TestbedPacket.h"

TESTBED_NAMESPACE_BEGIN

TestbedPacket::TestbedPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) : Packet(source, destination, sender, type, seqNr, ttl, payload, payloadSize) { }

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

// TODO
dessert_msg_t* TestbedPacket::getMessage() const {
    std::lock_guard<std::mutex> lock(mutex);
//    dessert_msg_t *result = new struct ether_header[payloadSize];
    // create a copy
  //  std::copy(dessertPayload, dessertPayload + payloadSize, result);
    // we simply return a copy
    //return result;
    return message;
}

void TestbedPacket::setMessage(dessert_msg_t* newMessage) {
    std::lock_guard<std::mutex> lock(mutex);

    if (message != nullptr){
       dessert_msg_destroy(message);
    }

    message = newMessage;
}

void TestbedPacket::setPayloadLength(unsigned int newPayloadLength){
    std::lock_guard<std::mutex> lock(mutex);
    /// FIXME: add a check for payload and payloadlength
    payloadSize = newPayloadLength;
}


dessert_msg_t* TestbedPacket::toDessertMessage() const {
    ara_address_t address;
    dessert_msg_t* packet = nullptr;
    dessert_ext_t* extension = nullptr;
    struct ether_header* ethernetHeader = nullptr;

    // get a libdessert representation of the destination address
    std::shared_ptr<TestbedAddress> destinationAddress = std::dynamic_pointer_cast<TestbedAddress>(destination);
    std::copy(destinationAddress->getDessertValue(), destinationAddress->getDessertValue() + ETHER_ADDR_LEN, address);

    // get a libdessert representation of the destination address
    std::shared_ptr<TestbedAddress> sourceAddress = std::dynamic_pointer_cast<TestbedAddress>(source);

    /// create a new dessert_msg_t
    if (dessert_msg_new(&packet) == 0) {
        /// set sequence number
        packet->u16 = htons(seqNr);
        /// set time to live 
        packet->ttl = (uint8_t)ttl;
        /// set the type
        packet->u8 |= type;
        // int antSize = ;
        
        dessert_msg_addext(packet, &extension, DESSERT_EXT_ETH, ETHER_HDR_LEN);
        ethernetHeader = (struct ether_header*) extension->data;

        if ((type == PacketType::FANT) || (type == PacketType::BANT)) {
            u_int8_t* src = sourceAddress->getDessertValue();
            u_int8_t* dest = destinationAddress->getDessertValue();

            /// TODO: better check this
            memcpy(ethernetHeader->ether_shost, src, ETHER_ADDR_LEN);
            memcpy(ethernetHeader->ether_dhost, dest, ETHER_ADDR_LEN);

            if (type == PacketType::FANT) {
                dessert_msg_addext(packet, &extension, ARA_EXT_FANT, ETHER_ADDR_LEN + 4);
                memcpy(extension->data, address, sizeof(ara_address_t));
                memcpy(extension->data + ETHER_ADDR_LEN, "FANT", 4);
            } else {
                dessert_msg_addext(packet, &extension, ARA_EXT_BANT, ETHER_ADDR_LEN + 4);
                memcpy(extension->data, address, sizeof(ara_address_t));
                memcpy(extension->data + ETHER_ADDR_LEN, "BANT", 4);
            }

            // dessert_msg_dummy_payload(packet, antSize);

        } else if (type == PacketType::DATA){
            if (message != nullptr) {
                /// we update the sequence number
                message->u16 = htons(seqNr);
                return message;
            }
        } else if (type == PacketType::PANT){

        } else if (type == PacketType::ACK){

        } else if (type == PacketType::DUPLICATE_ERROR){

        } else if (type == PacketType::ROUTE_FAILURE){

        } else if (type == PacketType::HELLO){

        } else if (type == PacketType::PEANT){

        } else {
            throw Exception("Unsupported packet type in makeAntAgent() method");
        }
    /// creating a dessert_msg_t failed
    } else {


    }

    return packet;
}

TESTBED_NAMESPACE_END
