/*
 * $FU-Copyright$
 */

#ifndef TESTBED_COMMAND_LINE_INTERFACE_H
#define TESTBED_COMMAND_LINE_INTERFACE_H

#include "Packet.h"
#include "StandardClock.h"

#include "CLibs.h"
#include "Testbed.h"
#include "Environment.h"
#include "TestbedARAClient.h"
#include "TestbedNetworkInterface.h"

TESTBED_NAMESPACE_BEGIN

/**
 * The CLI command prints the current content of the routing table 
 */
int cli_show_routing_table(struct cli_def* cli, const char* command, char* argv[], int argc);

/**
 * The CLI command prints the current set configuration of the routing daemon
 */
int cli_show_configuration(struct cli_def* cli, const char* command, char* argv[], int argc);

/**
 * The CLI command displays various statistics of the routing daemon
 */
int cli_show_statistics(struct cli_def* cli, const char* command, char* argv[], int argc);


//int cli_list_active_timers(struct cli_def* cli, char* command, char* argv[], int argc);

int cli_set_ack_timeout(struct cli_def* cli, const char* command, char* argv[], int argc);
//int cli_set_route_discovery_timeout(struct cli_def* cli, const char* command, char* argv[], int argc);


// TODO: check if this is still required
int cli_setup_meshif(struct cli_def* cli, const char* command, char* argv[], int argc);
// TODO: again check if this is still required
int cli_setup_sysif(struct cli_def* cli, const char* command, char* argv[], int argc);

int cli_test_send_mesh(struct cli_def* cli, char* command, char* argv[], int argc);


TESTBED_NAMESPACE_END

#endif 
