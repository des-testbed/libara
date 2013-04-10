/*
 * $FU-Copyright$
 */

#include <cstring>

#define CHECK_PACKET(packet, expectedType, expectedSeqNr, expectedTTL, source, sender, destination, payload)\
    BYTES_EQUAL(expectedType, packet->getType());\
    LONGS_EQUAL(expectedSeqNr, packet->getSequenceNumber());\
    CHECK(packet->getSource()->equals(source));\
    CHECK(packet->getSender()->equals(sender));\
    CHECK(packet->getDestination()->equals(destination));\
    LONGS_EQUAL(expectedTTL, packet->getTTL());\
    BYTES_EQUAL(std::strlen(payload)+1, packet->getPayloadLength());\
    STRCMP_EQUAL(payload, packet->getPayload());
