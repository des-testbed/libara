/*
 * $FU-Copyright$
 */

#include "CLibs.h"
#include "Packet.h"
#include "Testbed.h"
#include "TestbedARAClient.h"
#include "TestbedCommandLineInterface.h"

extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;

TESTBED_NAMESPACE_BEGIN

/*
int cli_list_active_timers(struct cli_def* cli, char* command, char* argv[], int argc) {
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());

    return CLI_OK;
}
*/

/*
*/

int cli_show_configuration(struct cli_def* cli, const char* command, char* argv[], int argc) {
    std::string configuration = client->toString();
    // print the configuration of the routing daemon
    cli_print(cli, "%s", configuration.c_str());
    return CLI_OK;
}

int cli_show_routing_table(struct cli_def* cli, const char* command, char* argv[], int argc) {
    // TODO: introduce semaphore for access to global client variable
    std::string routingTable = client->routingTableToString();
    // print the content of the routing table 
    cli_print(cli, "%s", routingTable.c_str());

    return CLI_OK;
}

int cli_show_statistics(struct cli_def* cli, const char* command, char* argv[], int argc) {
    /// dummy

    return CLI_OK;
}

/**
 * This is a wrapper function for initializing a mesh interface. This is necessary due to
 * recent changes in the libcli API (the called function is defined in libdessert).
 */
int cli_setup_meshif(struct cli_def* cli, const char* command, char* argv[], int argc) {
    return dessert_cli_cmd_addmeshif(cli, const_cast<char*>(command), argv, argc);
}

/**
 * This is a wrapper function for initializing a tap interface. This is necessary due to
 * recent changes in the libcli API (the called function is defined in libdessert).
 */
int cli_setup_sysif(struct cli_def* cli, const char* command, char* argv[], int argc) {
    return dessert_cli_cmd_addsysif(cli, const_cast<char*>(command), argv, argc);
}

/**
 *
 */
int cli_test_send_mesh(struct cli_def* cli, char* command, char* argv[], int argc){
    AddressPtr destination = std::make_shared<TestbedAddress>(DESSERT_BROADCAST_ADDRESS);
//    Packet* packet = client->getPacketFactory()->makeDataPacket(client->getNetworkInterface(0)->getLocalAddress(), destination, client->getNextSequenceNumber(), "test", 5);
//    client->sendPacket(packet);
    return CLI_OK;
}

TESTBED_NAMESPACE_END

