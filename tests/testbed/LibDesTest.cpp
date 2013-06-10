/*
 * $FU-Copyright$
 */
#include <iostream>
#include "CppUTest/TestHarness.h"
#include "testbed/CLibs.h"



TEST_GROUP(LibDesTest) {};

TEST(LibDesTest, createDaemon) {
    dessert_init("expl", 0x05, DESSERT_OPT_DAEMONIZE);
    cli_init();
    dessert_cli_run();
    dessert_run();
    //check if ini file was created?
}
