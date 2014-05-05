/*
 * $FU-Copyright$
 */

#ifndef TESTBED_COMMAND_LINE_INTERFACE_H
#define TESTBED_COMMAND_LINE_INTERFACE_H

#include "CLibs.h"
#include "Testbed.h"
#include "Packet.h"
#include "testbed/TestbedNetworkInterface.h"
#include "TestbedARAClient.h"

TESTBED_NAMESPACE_BEGIN

//int cli_list_active_timers(struct cli_def* cli, const char* command, char* argv[], int argc);

// TODO: check if this is still required
int cli_setup_meshif(struct cli_def* cli, const char* command, char* argv[], int argc);
// TODO: again check if this is still required
int cli_setup_sysif(struct cli_def* cli, const char* command, char* argv[], int argc);

int cli_test_send_mesh(struct cli_def* cli, char* command, char* argv[], int argc);


TESTBED_NAMESPACE_END

#endif 
