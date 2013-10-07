/*
 * $FU-Copyright$
 */
#ifndef ENVIRONMENT_PLUGIN_H_
#define ENVIRONMENT_PLUGIN_H_

#include "Environment.h"
#include "testAPI/mocks/time/ClockMock.h"

#include "CppUTest/TestHarness.h"

class EnvironmentPlugin : public TestPlugin {
    public:
        EnvironmentPlugin(SimpleString name);
        void preTestAction(UtestShell&, TestResult&);
        void postTestAction(UtestShell&, TestResult&);
};

#endif

