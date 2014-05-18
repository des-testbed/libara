/*
 * $FU-Copyright$
 */

#include "TestbedMain.h"

using namespace std;


/**
 *
 */
ARA::BasicConfiguration createConfiguration(double deltaPhi, double initialPhi) {
    int maxTTL = 15;
    ARA::testbed::TestbedRoutingTable* routingTable = new ARA::testbed::TestbedRoutingTable();
    ARA::testbed::TestbedPacketFactory* packetFactory = new ARA::testbed::TestbedPacketFactory(maxTTL);
    ARA::EvaporationPolicy* evaporationPolicy = new ARA::ExponentialEvaporationPolicy(2.0, 100, 15.0);
    ARA::PathReinforcementPolicy* reinforcementPolicy = new ARA::LinearPathReinforcementPolicy(deltaPhi);
    ARA::ForwardingPolicy* forwardingPolicy = new ARA::BestPheromoneForwardingPolicy(routingTable);
    return ARA::BasicConfiguration(routingTable, packetFactory, evaporationPolicy, reinforcementPolicy, forwardingPolicy, initialPhi);
}


static void registerCommandLineInterfaceCommands(){
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("sys"), dessert_cli_cmd_addsysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize tap interface"));
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("mesh"), dessert_cli_cmd_addmeshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize mesh interface"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("testSendMesh"), ARA::testbed::cli_test_send_mesh, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("send a test packet to mesh interface"));
}

static void registerCallbacks(){
     /// send with dessert_meshsend() data over a registered mesh interface
     dessert_cb_result (*fromTAP)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_sysif_t*, dessert_frameid_t) = &ARA::testbed::toMesh;
     ///
     dessert_cb_result (*packetFilter)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_sysif_t*, dessert_frameid_t) = &ARA::testbed::packetFilter;
     ///
     dessert_cb_result (*fromMesh)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_meshif_t*, dessert_frameid_t) = &ARA::testbed::toSys;

     ///
     dessert_sysrxcb_add(packetFilter, 5);
     ///
     dessert_sysrxcb_add(fromTAP, 15);
     ///
     dessert_meshrxcb_add(dessert_msg_ifaceflags_cb, 15);
     ///
     dessert_meshrxcb_add(fromMesh, 30);
}

int main(int argc, char** argv) {
     FILE* cfg = dessert_cli_get_cfg(argc, argv);

     dessert_init("ARA", 0x01, DESSERT_OPT_NODAEMONIZE);
     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); 

     registerCommandLineInterfaceCommands();

     dessert_debug("applying configuration");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     dessert_debug("configuration applied");

     ARA::BasicConfiguration config = createConfiguration(5.0, 5.0);
     client = std::make_shared<ARA::testbed::TestbedARAClient>(config);

     registerCallbacks();

     dessert_cli_run();
     dessert_run();

     return 0;
}
