/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "TestbedARAClient.h"
#include "PacketDispatcher.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClient::~TestbedARAClient() {

}

void TestbedARAClient::sendPacket(Packet* packet) {
    if(routingTable->isDeliverable(packet)) {
        NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
        nextHop->getInterface()->send(packet, nextHop->getAddress());
    }
    this->sendFANT(packet->getDestination());
}

void TestbedARAClient::receivePacket(Packet* packet, ARA::NetworkInterface* interface) {
    AbstractARAClient::receivePacket(packet, interface);
    //TODO: persistRoutingTableData
}

void TestbedARAClient::deliverToSystem(const Packet* packet) {
    dessert_syssend_msg(extractDessertMessage(packet));
}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {
    delete packet;
    //nrOfNotDeliverablePackets++;
}


TESTBED_NAMESPACE_END
