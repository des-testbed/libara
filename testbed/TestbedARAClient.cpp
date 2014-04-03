/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "CLibs.h"
#include "TestbedARAClient.h"
#include "TestbedPacketDispatcher.h"
#include "BasicConfiguration.h"
#include "SimpleLogger.h"
#include "Environment.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClient::TestbedARAClient(Configuration& config) : AbstractARAClient(config){
    //TODO Make configurable
    Logger* logger = new SimpleLogger("ara");
    setLogger(logger);
    logDebug("Initialized testbedARAClient");
    initializeNetworkInterfaces();
    logDebug("Initialized testbedARAClient network Interfaces");
}

TestbedARAClient::~TestbedARAClient() { }

void TestbedARAClient::sendPacket(Packet* packet) {
    logDebug("will send packet");
    AbstractARAClient::sendPacket(packet);
}

void TestbedARAClient::receivePacket(Packet* packet, ARA::NetworkInterface* interface) {
    logDebug("receiving packet # %u type %s over interface at %s", packet->getSequenceNumber(), PacketType::getAsString(packet->getType()).c_str(), interface->getLocalAddress()->toString().c_str());
    AbstractARAClient::receivePacket(packet, interface);
    //TODO: persistRoutingTableData
}

void TestbedARAClient::deliverToSystem(const Packet* packet) {
    logDebug("sending packet # %u to System via TAP", packet->getSequenceNumber());
    std::cout << "deliverToSystem " << packet->getPayload() << " delivered to system." << std::endl;
    dessert_syssend((void*) packet->getPayload(), packet->getPayloadLength());
}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {
    logDebug("packet # %u is undeliverable", packet->getSequenceNumber());
    delete packet;
    //nrOfNotDeliverablePackets++;
}

void TestbedARAClient::initializeNetworkInterfaces() {
    dessert_meshif_t* dessertInterfaces = dessert_meshiflist_get();
    while(dessertInterfaces != NULL) {
	TestbedNetworkInterface* newInterface = new TestbedNetworkInterface(dessertInterfaces, this, packetFactory, 400000);
        addNetworkInterface(newInterface);
        logDebug("initialized network interface: %s", dessertInterfaces->if_name);
        dessertInterfaces = dessertInterfaces->next;
    }
    tapAddress = std::make_shared<TestbedAddress>(dessert_l25_defsrc);
    std::cout << "[initializeNetworkInterfaces] tap address is: " << tapAddress->toString() << std::endl;
}

bool TestbedARAClient::isLocalAddress(AddressPtr address) const {
    std::cout << "address is "  << address->toString() << std::endl;
    std::cout << "other address is "  << tapAddress->toString() << std::endl;
    return(address.get()->equals(tapAddress) || AbstractNetworkClient::isLocalAddress(address));
}

void TestbedARAClient::broadcastFANT(AddressPtr destination) {
    unsigned int sequenceNr = getNextSequenceNumber();

    for(auto& interface: interfaces) {
        Packet* fant = packetFactory->makeFANT(tapAddress, destination, sequenceNr);
        interface->broadcast(fant);
    }
}

TESTBED_NAMESPACE_END
