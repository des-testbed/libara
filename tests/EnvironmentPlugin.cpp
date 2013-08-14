#include "EnvironmentPlugin.h"

EnvironmentPlugin::EnvironmentPlugin(SimpleString name) : TestPlugin(name) { }

void EnvironmentPlugin::preTestAction(UtestShell& shell, TestResult& result) {
    ARA::Environment::setClock(new ARA::ClockMock());
}

void EnvironmentPlugin::postTestAction(UtestShell& shell, TestResult& result) {
    ARA::Environment::setClock(nullptr);
}
