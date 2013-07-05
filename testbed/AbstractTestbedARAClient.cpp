/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "AbstractTestbedARAClient.h"

TESTBED_NAMESPACE_BEGIN

AbstractTestbedARAClient::~AbstractTestbedARAClient() {

}

void AbstractTestbedARAClient::sendPacket(Packet* packet) {

}

void AbstractTestbedARAClient::receivePacket(Packet* packet, NetworkInterface* interface) {

}

bool AbstractTestbedARAClient::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    return false;
}

void AbstractTestbedARAClient::timerHasExpired(Timer* responsibleTimer) {

}

void AbstractTestbedARAClient::handlePacket(Packet* packet, NetworkInterface* interface) {

}

void AbstractTestbedARAClient::handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface) {

}

void AbstractTestbedARAClient::startNewRouteDiscovery(Packet* packet) {

}

void AbstractTestbedARAClient::handleNonSourceRouteDiscovery(Packet* packet) {

}

void AbstractTestbedARAClient::handlePacketWithZeroTTL(Packet* packet) {

}

TESTBED_NAMESPACE_END
