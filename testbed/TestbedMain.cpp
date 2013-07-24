/*
 * $FU-Copyright$
 */

#include "CLibs.h"
#include "PacketDispatcher.h"
#include "TestbedTimer.h"
#include "TestbedARAClient.h"
#include "BasicConfiguration.h"
#include "LinearPathReinforcementPolicy.h"
#include "BestPheromoneForwardingPolicy.h"

typedef u_char ara_address_t[ETHER_ADDR_LEN];

using namespace std;

ARA::testbed::TestbedARAClient* client;

ARA::BasicConfiguration createConfiguration(double deltaPhi, double initialPhi) {
    return ARA::BasicConfiguration(new ARA::RoutingTable(), new ARA::PacketFactory(15), new ARA::ExponentialEvaporationPolicy(2.0, 100, 15.0),
                                            new ARA::LinearPathReinforcementPolicy(deltaPhi), new ARA::BestPheromoneForwardingPolicy(), initialPhi);
}

_dessert_cb_results messageFromTapInterfaceDispatcher(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_sysif_t *interface, dessert_frameid_t id) {
    ether_header* ethernetFrame = ARA::testbed::extractEthernetHeader(messageReceived);
    ARA::testbed::addRoutingExtension(messageReceived, interface->hwaddr, ethernetFrame->ether_dhost);
    ARA::Packet* packet = ARA::testbed::extractPacket(messageReceived);
    client->sendPacket(packet);
    return DESSERT_MSG_DROP;
}

int testbed_cli_cmd_testsendmesh(struct cli_def* cli, char* command, char* argv[], int argc){
    ARA::AddressPtr destination = ARA::AddressPtr(new ARA::testbed::TestbedAddress(DESSERT_BROADCAST_ADDRESS));
    ARA::Packet* packet = client->getPacketFactory()->makeDataPacket(client->getNetworkInterface(0)->getLocalAddress(), destination, client->getNextSequenceNumber(), "test", 5);
    client->sendPacket(packet);
    return 0;
}

int main(int argc, char** argv) {
     FILE* cfg = dessert_cli_get_cfg(argc, argv);

     dessert_init("ARA", 0x01, DESSERT_OPT_NODAEMONIZE);

     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); 

     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("sys"), dessert_cli_cmd_addsysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize tap interface"));
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("mesh"), dessert_cli_cmd_addmeshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize mesh interface"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("testSendMesh"), testbed_cli_cmd_testsendmesh, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("send a test packet to mesh interface"));

     dessert_cb_result (*fromTAP)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_sysif_t*, dessert_frameid_t) = &messageFromTapInterfaceDispatcher;
     _dessert_cb_results (*fromMesh)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_meshif_t*, dessert_frameid_t) = &ARA::testbed::messageFromMeshInterfaceDispatcher;

     dessert_debug("applying configuration");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     dessert_debug("configuration applied");

     ARA::BasicConfiguration config = createConfiguration(5.0, 5.0);
     client = new ARA::testbed::TestbedARAClient(config);

     dessert_sysrxcb_add(fromTAP, 15);
     dessert_meshrxcb_add(dessert_msg_ifaceflags_cb, 15);
     dessert_meshrxcb_add(fromMesh, 30);

     dessert_cli_run();
     dessert_run();

     return 0;
 }
