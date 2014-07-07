/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "CppUTest/TestHarness.h"
#include "TestbedPacketFactory.h"
#include "testAPI/mocks/AddressMock.h"
#include "TestbedPacket.h"
#include "TestbedAddress.h"
#include "PacketType.h"

#include <cstring>

TESTBED_NAMESPACE_BEGIN

TEST_GROUP(TestbedPacketFactoryTest) {
    TestbedPacketFactory* factory;
    int maximumHopCount= 15;
  
    void setup() {
        factory = new TestbedPacketFactory(maximumHopCount);
    }

    void teardown() {
        delete factory;
    }
};

TEST(TestbedPacketFactoryTest, makeDataPacket) {
    u_char sourceMac[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    std::shared_ptr<TestbedAddress> source = std::make_shared<TestbedAddress>(sourceMac);

    u_char destinationMac[ETHER_ADDR_LEN] = {0,2,3,4,5,6};
    std::shared_ptr<TestbedAddress> destination = std::make_shared<TestbedAddress>(destinationMac);

    unsigned int seqNr = 42;
    const char *payload = "makePacketTest";
    unsigned int payloadSize = 15;

    TestbedPacket* packet = factory->makeDataPacket(source, destination, seqNr, payload, payloadSize);

    CHECK(packet->getSequenceNumber() == seqNr);
    CHECK(packet->getSource() == source);
    CHECK(packet->getDestination() == destination);
    CHECK(packet->getType() == PacketType::DATA);

    std::string payloadString("makePacketTest");
    CHECK(payloadString.compare(packet->getPayload()) == 0);
    CHECK(packet->getPayloadLength() == payloadSize);

    delete packet;

    dessert_msg_t* anotherPayload = nullptr;
    dessert_msg_new(&anotherPayload);
    payloadSize = sizeof(dessert_msg_t);

    TestbedPacket* anotherPacket = factory->makeDataPacket(source, destination, seqNr, anotherPayload, payloadSize);
    CHECK(anotherPacket->getSequenceNumber() == seqNr);
    CHECK(anotherPacket->getSource() == source);
    CHECK(anotherPacket->getMessage() == anotherPayload);
    CHECK(anotherPacket->getDestination() == destination);
    CHECK(anotherPacket->getType() == PacketType::DATA);
    CHECK(packet->getPayloadLength() == payloadSize);

    delete anotherPacket;
    dessert_msg_destroy(anotherPayload);
}

TEST(TestbedPacketFactoryTest, makeClone) {
    u_char sourceMac[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    std::shared_ptr<TestbedAddress> source = std::make_shared<TestbedAddress>(sourceMac);

    u_char destinationMac[ETHER_ADDR_LEN] = {0,2,3,4,5,6};
    std::shared_ptr<TestbedAddress> destination = std::make_shared<TestbedAddress>(destinationMac);

    unsigned int seqNr = 42;
    const char *payload = "makePacketTest";
    unsigned int payloadSize = 15;

    TestbedPacket* packet = factory->makeDataPacket(source, destination, seqNr, payload, payloadSize);
    TestbedPacket* clonedPacket = factory->makeClone(packet);

    CHECK(packet->getSource() == clonedPacket->getSource());
    CHECK(packet->getSender() == clonedPacket->getSender());
    CHECK(packet->getDestination() == clonedPacket->getDestination());
    CHECK(packet->getSequenceNumber() == clonedPacket->getSequenceNumber());
    CHECK(packet->getPayloadLength() == clonedPacket->getPayloadLength());

    std::string payloadString("makePacketTest");
    CHECK(payloadString.compare(packet->getPayload()) == 0);
    CHECK(payloadString.compare(clonedPacket->getPayload()) == 0);

    dessert_msg_t* anotherPayload = nullptr;
    dessert_msg_new(&anotherPayload);
    payloadSize = sizeof(dessert_msg_t);

    TestbedPacket* anotherPacket = factory->makeDataPacket(source, destination, seqNr, anotherPayload, payloadSize);
    TestbedPacket* anotherClonedPacket = factory->makeClone(anotherPacket);

    CHECK(anotherClonedPacket->getSequenceNumber() == anotherPacket->getSequenceNumber());
    CHECK(anotherClonedPacket->getSource() == anotherPacket->getSource());
    CHECK(anotherClonedPacket->getMessage() == anotherPayload);
    CHECK(anotherClonedPacket->getDestination() == anotherPacket->getDestination());
    CHECK(anotherClonedPacket->getPayloadLength() == anotherPacket->getPayloadLength());

    delete packet;
    delete clonedPacket;
    delete anotherPacket;
    delete anotherClonedPacket;

    dessert_msg_destroy(anotherPayload);
}
/*

e 6 on interface wlan0
l2_dhost: ff:ff:ff:ff:ff:ff
l2_shost: 00:1f:1f:09:09:e2
l2_type:   88b5

proto:     ARA
ver:       1
flags:    
ttl:  f
u8:  1
u16:  200
hlen:      60
plen:      0

l25 proto: ethernet
l25_dhost: ff:ff:ff:ff:ff:ff
l25_shost: 00:1f:1f:09:09:e2
l25_type:  0

extension 0:
type:      0x40
len:       16
data:      0xfe 0xd 0xb9 0x14 0xd8 0xc8 0x0 0x1f 0x1f 0x9 0x9 0xe2 0x0 0x0                         
extension 1:
type:      0x01
len:       16

local processing header:
lflags:     DESSERT_FLAG_DST_BROADCAST NEXTHOP_BROADCAST
*/
/*
TEST(TestbedPacketFactoryTest, makeDessertMessage) {
    /// set up the source, destination and sender addresses
    u_char sourceMac[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    TestbedAddressPtr source = std::make_shared<TestbedAddress>(sourceMac);
 
    u_char destinationMac[ETHER_ADDR_LEN] = {7,8,9,0xA,0xB,0xC};
    TestbedAddressPtr destination = std::make_shared<TestbedAddress>(destinationMac);

    u_char nextHopMac[ETHER_ADDR_LEN] = {0xD,0xE,0xF,4,5,6};
    TestbedAddressPtr nextHop = std::make_shared<TestbedAddress>(nextHopMac);

    int sequenceNumber = 4223;

    /// let's first try it with no payload
    dessert_msg_t *emptyMessage = nullptr;
    int payloadSize = 0;

    /// create a data packet
    Packet *packet = factory->makeDataPacket(source, destination, sequenceNumber, emptyMessage, payloadSize);
    CHECK(packet->getSource()->equals(source));
    CHECK(packet->getDestination()->equals(destination));
    CHECK(packet->getType() == PacketType::DATA);

    /// create a mesh interface
    dessert_meshif_t* interface = new dessert_meshif_t();

    // make the data packet a dessert message
    dessert_msg_t* message = factory->makeDessertMessage(const_cast<const Packet*>(packet), interface, nextHop);

    CHECK(message != nullptr);

    TestbedAddress ethernetHeaderSource(message->l2h.ether_shost);
    TestbedAddress ethernetHeaderDestination(message->l2h.ether_dhost);

    u_char ethernetSourceMac[ETHER_ADDR_LEN] = {0,0,0,0,0,0};
    TestbedAddressPtr ethernetSource = std::make_shared<TestbedAddress>(ethernetSourceMac);
    TestbedAddressPtr ethernetDestination = std::make_shared<TestbedAddress>(ethernetSourceMac);

    std::cerr << " 1 " << ethernetHeaderSource << std::endl;
    std::cerr << " 2 " << ethernetHeaderDestination << std::endl;

    CHECK(ethernetHeaderSource == ethernetSource);
    CHECK(ethernetHeaderDestination == ethernetDestination);
    CHECK(message->l2h.ether_dhost[5] == destinationMac[5]);
    CHECK(message->u16 == sequenceNumber);
    CHECK(message->ttl == 15);
    CHECK(message->u8 == PacketType::DATA);
    CHECK(message->plen == payloadSize);

    delete interface;
    delete packet;
   // delete message;
}
*/

TEST(TestbedPacketFactoryTest, makeFANT) {
    u_char sourceMac[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    TestbedAddressPtr source = std::make_shared<TestbedAddress>(sourceMac);
 
    u_char destinationMac[ETHER_ADDR_LEN] = {7,8,9,0xA,0xB,0xC};
    TestbedAddressPtr destination = std::make_shared<TestbedAddress>(destinationMac);

    unsigned int newSequenceNumber = 242342;
    Packet* fant = factory->makeFANT(source, destination, newSequenceNumber);

    CHECK(fant->getSource()->equals(source));
    CHECK(fant->getDestination()->equals(destination));
    CHECK(fant->getSender()->equals(source));
    CHECK(fant->getPreviousHop()->equals(source));
    CHECK(fant->getType() == PacketType::FANT);

    LONGS_EQUAL(newSequenceNumber, fant->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, fant->getTTL());
    LONGS_EQUAL(0, fant->getPayloadLength());

    delete fant;
}

TEST(TestbedPacketFactoryTest, makeBANT) {
    u_char sourceMac[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    TestbedAddressPtr originalSource = std::make_shared<TestbedAddress>(sourceMac);

    u_char destinationMac[ETHER_ADDR_LEN] = {7,8,9,0xA,0xB,0xC};
    TestbedAddressPtr originalDestination = std::make_shared<TestbedAddress>(destinationMac);

    int seqNr = 3;

    TestbedPacket* fant = factory->makeFANT(originalSource, originalDestination, seqNr);
    unsigned int newSequenceNumber = 12345;
    TestbedPacket* bant = factory->makeBANT(fant, newSequenceNumber);

    CHECK(bant->getSource()->equals(originalDestination));
    CHECK(bant->getDestination()->equals(originalSource));
    CHECK(bant->getSender()->equals(originalDestination));
    CHECK(bant->getPreviousHop()->equals(originalDestination));
    CHECK(bant->getType() == PacketType::BANT);

    LONGS_EQUAL(newSequenceNumber, bant->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, bant->getTTL());
    LONGS_EQUAL(0, bant->getPayloadLength());

    delete fant;
    delete bant;
}
/*
TEST(TestbedPacketFactoryTest, makeAcknowledgmentPacket) {
    u_char sourceMac[ETHER_ADDR_LEN] = {1,2,3,4,5,6};
    TestbedAddressPtr originalSource = std::make_shared<TestbedAddress>(sourceMac);

    u_char destinationMac[ETHER_ADDR_LEN] = {7,8,9,0xA,0xB,0xC};
    TestbedAddressPtr originalDestination = std::make_shared<TestbedAddress>(destinationMac);

    u_char senderMac[ETHER_ADDR_LEN] = {0xD,0xE,0xF,4,5,6};
    TestbedAddressPtr originalSender = std::make_shared<TestbedAddress>(senderMac);

    unsigned int type = PacketType::DATA;
    unsigned int originalseqenceNumber = 123;
    unsigned int originalTTL = 10;

    TestbedPacket packet(originalSource, originalDestination, originalSender, type, originalseqenceNumber, originalTTL);
    TestbedPacket* ackPacket = factory->makeAcknowledgmentPacket(&packet, originalSender);

    CHECK(ackPacket->getSource()->equals(originalSource));
    CHECK(ackPacket->getDestination()->equals(originalDestination));
    CHECK(ackPacket->getSender()->equals(originalSender));
    CHECK(ackPacket->getPreviousHop()->equals(originalSender));
    CHECK(ackPacket->getType() == PacketType::ACK);
    LONGS_EQUAL(originalseqenceNumber, ackPacket->getSequenceNumber());
    LONGS_EQUAL(maximumHopCount, ackPacket->getTTL());
    LONGS_EQUAL(0, ackPacket->getPayloadLength());

    delete ackPacket;
}
*/

/*
TEST(TestbedPacketFactoryTest, makeDessertMessageFANT){

}
*/

TESTBED_NAMESPACE_END
