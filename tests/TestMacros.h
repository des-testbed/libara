/*
 * $FU-Copyright$
 */

#include <cstring>

#define CHECK_PACKET(packet, expectedType, expectedSeqNr, source, sender, destination, hopCount, payload)\
    BYTES_EQUAL(expectedType, packet->getType());\
    LONGS_EQUAL(expectedSeqNr, packet->getSequenceNumber());\
    CHECK(packet->getSource()->equals(source));\
    CHECK(packet->getSender()->equals(sender));\
    CHECK(packet->getDestination()->equals(destination));\
    CHECK(packet->getHopCount() == hopCount);\
    BYTES_EQUAL(std::strlen(payload)+1, packet->getPayloadLength());\
    STRCMP_EQUAL(payload, packet->getPayload());
