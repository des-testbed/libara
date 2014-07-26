/*
 * $FU-Copyright$
 */

#include "TestbedMain.h"

using namespace std;


/**
 * The function creates a basic configuration for the ARA routing daemon.
 */
ARA::BasicConfiguration createConfiguration(double deltaPhi, double initialPhi) {
    int maxTTL = 15;
    ARA::testbed::TestbedRoutingTable* routingTable = new ARA::testbed::TestbedRoutingTable();
    ARA::testbed::TestbedPacketFactory* packetFactory = new ARA::testbed::TestbedPacketFactory(maxTTL);
    ARA::EvaporationPolicy* evaporationPolicy = new ARA::ExponentialEvaporationPolicy(2.0, 100, 15.0);
    ARA::PathReinforcementPolicy* reinforcementPolicy = new ARA::LinearPathReinforcementPolicy(deltaPhi);
    ARA::ForwardingPolicy* forwardingPolicy = new ARA::BestPheromoneForwardingPolicy(routingTable);
    std::shared_ptr<ARA::testbed::TestbedPacketTrap> packetTrap = std::make_shared<ARA::testbed::TestbedPacketTrap>(routingTable);
    return ARA::BasicConfiguration(routingTable, packetFactory, evaporationPolicy, reinforcementPolicy, forwardingPolicy, packetTrap, initialPhi);
}

/**
 * The function registers the callback functions for the remote shell provided
 * by libcli. The (callback) functions are defined in
 * TestbedCommandLineInterface.h and implemented in
 * TestbedCommandLineInterface.cpp
 */
static void registerCommandLineInterfaceCommands(){
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("sys"), dessert_cli_cmd_addsysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize tap interface"));
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("mesh"), dessert_cli_cmd_addmeshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize mesh interface"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("testSendMesh"), ARA::testbed::cli_test_send_mesh, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("send a test packet to mesh interface"));
     //cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("list timers"), ARA::testbed::cli_list_active_timers, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("list active timers"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("routing table"), ARA::testbed::cli_show_routing_table, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("displays the content of the routing table"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("configuration"), ARA::testbed::cli_show_configuration, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("lists the current set configuration"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("statistics"), ARA::testbed::cli_show_statistics, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("shows various statistics of the routing daemon"));
}

/**
 * The function registers the callbacks for libdessert. The (callback) functions
 * are defined in TestbedPacketDispatcher.h and implemented in
 * TestbedPacketDispatcher.cpp. The naming of the functions follows the example
 * routing daemon in the libdessert documentation.
 */
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
     /// get a file pointer to the configuration file
     FILE* cfg = dessert_cli_get_cfg(argc, argv);

     /// initialize the routing daemin in dessert
     dessert_init("ARAX", 0x01, DESSERT_OPT_NODAEMONIZE);
     //dessert_init("ARA", 0x01, DESSERT_OPT_NODAEMONIZE);
     /// configuration options for the logging mechanisms provided by libdessert
     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); 
     
     /// register the remote shell commands
     registerCommandLineInterfaceCommands();

     /// apply the routing daemon configuration
     dessert_debug("applying configuration");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     dessert_debug("configuration applied");

     // create a new configuration for ARA and pass it to the client
     ARA::BasicConfiguration configuration = createConfiguration(5.0, 5.0);
     client = std::make_shared<ARA::testbed::TestbedARAClient>(configuration);

     /// register the callbacks for the packet dispatching mechanisms
     registerCallbacks();

     dessert_cli_run();
     dessert_run();

     return 0;
}
