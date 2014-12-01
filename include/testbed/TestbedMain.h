/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_MAIN_H_
#define _TESTBED_MAIN_H_

#include "CLibs.h"
#include "Testbed.h"
#include "PacketType.h"
#include "TestbedAddress.h"
#include "TestbedARAClient.h"
#include "TestbedPacketTrap.h"
#include "BasicConfiguration.h"
#include "TestbedRoutingTable.h"
#include "TestbedPacketFactory.h"
#include "TestbedPacketDispatcher.h"
#include "TestbedCommandLineInterface.h"
#include "LinearPathReinforcementPolicy.h"
#include "BestPheromoneForwardingPolicy.h"

#include <mutex>

//std::shared_ptr<ARA::testbed::TestbedARAClient> client;
ARA::testbed::TestbedARAClient *client;

TESTBED_NAMESPACE_BEGIN

//std::mutex clientMutex;

/**
 * The function allocates memory for TestbedARAClient global
 * variable
 *
 * @param configuration The settings for the new TestbedARAClient
 */
void initializeClient(ARA::BasicConfiguration configuration);


void destroyClient();


TESTBED_NAMESPACE_END

#endif 
