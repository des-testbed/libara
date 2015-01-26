/*
 * $FU-Copyright$
 */

#include "TestbedMain.h"

using namespace std;

/**
 * The function creates a basic configuration for the ARA routing daemon.
 */
ARA::BasicConfiguration createConfiguration(double deltaPhi, double initialPhi) {
    int maxTTL = 255;
    ARA::testbed::TestbedRoutingTable* routingTable = new ARA::testbed::TestbedRoutingTable();
    ARA::testbed::TestbedPacketFactory* packetFactory = new ARA::testbed::TestbedPacketFactory(maxTTL);
    ARA::EvaporationPolicy* evaporationPolicy = new ARA::ExponentialEvaporationPolicy(0.05, 100, 5.0);
    ARA::PathReinforcementPolicy* reinforcementPolicy = new ARA::LinearPathReinforcementPolicy(deltaPhi);
    ARA::ForwardingPolicy* forwardingPolicy = new ARA::BestPheromoneForwardingPolicy(routingTable);
    std::shared_ptr<ARA::testbed::TestbedPacketTrap> packetTrap = std::make_shared<ARA::testbed::TestbedPacketTrap>(routingTable);
    return ARA::BasicConfiguration(routingTable, packetFactory, evaporationPolicy, reinforcementPolicy, forwardingPolicy, packetTrap, initialPhi);
}

void initializeClient(ARA::BasicConfiguration configuration){
    if (!client) {
        client = new ARA::testbed::TestbedARAClient(configuration);
    } 
}

void destroyClient(){
    if (client) {
        delete client;
    } 
}

/**
 * The function registers the callback functions for the remote shell provided
 * by libcli. 
 */
static void registerCommandLineInterfaceCommands() {
    cli_register_command(dessert_cli, dessert_cli_cfg_iface, "sys", ARA::testbed::cli_setup_sysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "initialize tap interface");
    cli_register_command(dessert_cli, dessert_cli_cfg_iface, "mesh", ARA::testbed::cli_setup_meshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "initialize mesh interface");
    cli_register_command(dessert_cli, dessert_cli_show, "ack timeout", ARA::testbed::cli_set_ack_timeout, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "set acknowledgement timer timeout [ms]");
    cli_register_command(dessert_cli, dessert_cli_show, "routing table", ARA::testbed::cli_show_routing_table, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "displays the content of the routing table");
    cli_register_command(dessert_cli, dessert_cli_show, "configuration", ARA::testbed::cli_show_configuration, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "displays the current configuration");
    cli_register_command(dessert_cli, dessert_cli_show, "statistics", ARA::testbed::cli_show_statistics, PRIVILEGE_UNPRIVILEGED, MODE_ANY, "shows various statistics of the routing daemon");
}

/**
 * The function registers the callbacks for libdessert. The (callback) functions
 * are defined in TestbedPacketDispatcher.h and implemented in
 * TestbedPacketDispatcher.cpp. The naming of the functions follows the example
 * routing daemon in the libdessert documentation.
 */
static void registerCallbacks() {
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
    namespace spd = spdlog;

    try {
        /// set log level of all loggers to debug and above
        spd::set_level(spd::level::debug);

        char hostName[64];

        /// determine host name 
        if (gethostname(hostName, sizeof(hostName)) != 0) {
            auto console = spd::stdout_logger_mt("console");
            console->error() << "could not get host name! errno " << errno << ": ";
            console->error() << strerror(errno) << 1;
        } 

        std::stringstream logFileName;
        logFileName << "logs/" << hostName << ".log";

        std::string logFile = logFileName.str();

        /// Create a file rotating logger with 5mb size max and 3 rotated files
        auto file_logger = spd::rotating_logger_mt("file_logger", logFile, 1048576 * 5, 3);

        /// get a file pointer to the configuration file
        FILE* cfg = dessert_cli_get_cfg(argc, argv);

        /// initialize the routing daemin in dessert
        dessert_init("ARAX", 0x01, DESSERT_OPT_NODAEMONIZE);
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
        initializeClient(configuration);

	    /// register the callbacks for the packet dispatching mechanisms
	    registerCallbacks();

        /// start the cli and libdessert
        dessert_cli_run();
        dessert_run();

	    /// clean up (destroying the client instance)
	    destroyClient();

        return 0;

    } catch (const spd::spdlog_ex& ex) {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
    
    return -1;
}
