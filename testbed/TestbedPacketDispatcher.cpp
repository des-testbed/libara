/*
 * $FU-Copyright$
 */

#include "TestbedPacket.h"
#include "TestbedPacketDispatcher.h"
#include "TestbedAddress.h"

#include <netinet/in.h>

extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;

TESTBED_NAMESPACE_BEGIN

/**
 * RECEIVE
 */
dessert_cb_result toSys(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_meshif_t* interface, dessert_frameid_t id) {
    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());
    // DEBUG: std::cout << "[messageFromMeshInterfaceDispatcher] heyho i've got a packet" << std::endl;
    TestbedPacket* packet = packetFactory->makeNewPacket(message);
    // DEBUG: std::cout << " Packet Dump " << std::endl;
    // DEBUG: std::cout << packet;

    TestbedNetworkInterface* networkInterface = client->getTestbedNetworkInterface(std::make_shared<TestbedAddress>(interface->hwaddr));
    networkInterface->receive(packet);

    return DESSERT_MSG_DROP;
}

/**
 * SEND
 */
int _send(const Packet* packet, std::shared_ptr<TestbedAddress> interfaceAddress, std::shared_ptr<Address> recipient) {
    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());
    /// determine the mesh interface
    dessert_meshif_t* interface = dessert_meshif_get_hwaddr(interfaceAddress->getDessertValue());
    //
    dessert_msg_t* message = packetFactory->makeDessertMessage(packet, interface, recipient);
    ///
    dessert_meshsend(message, interface);
    ///
    return DESSERT_MSG_DROP;
}


dessert_cb_result toMesh(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_sysif_t *interface, dessert_frameid_t id) {
    dumpDessertMessage(message, length, flags, interface, id);

    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());
    // DEBUG: std::cout << "[messageFromMeshInterfaceDispatcher] heyho i've got a packet" << std::endl;
    TestbedPacket* packet = packetFactory->makeDataPacket(message);
    std::cout << "Packet Dump:" << std::endl;
    std::cout << *packet << std::endl;
    client->sendPacket(packet);
    return DESSERT_MSG_DROP;
}

void dumpDessertMessage(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_sysif_t *interface, dessert_frameid_t id) {
    char buffer[1024];
    dessert_msg_proc_dump(message, length, flags, buffer, 1024);
    std::cout << "received frame " << ((unsigned long)id) << " on interface " << interface->if_name << std::endl;
    std::cout << buffer;
    std::cout << std::endl;
}

dessert_cb_result packetFilter(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_sysif_t *interface, dessert_frameid_t id) {
    dessert_ext_t* extension;
    dessert_msg_getext(message, &extension, DESSERT_EXT_ETH, 0);
    ether_header* ethernetFrame = (ether_header*) extension->data;

    if(ethernetFrame->ether_dhost[0] == 51) {
        return DESSERT_MSG_DROP;
    } else {
        return DESSERT_MSG_KEEP;
    }
}

TESTBED_NAMESPACE_END
