/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "Environment.h"
#include "Exception.h"
#include "testAPI/plugins/EnvironmentPlugin.h"
#include <iostream>

#include "spdlog/spdlog.h"

using namespace ARA;

int main(int ac, char** av) {
    namespace spd = spdlog;

    try {
        TestRegistry* r = TestRegistry::getCurrentRegistry();
        /// log filename is unit_test.txt
        std::string logFile("unit_test");

        auto file_logger = spd::rotating_logger_mt("file_logger", logFile, 1048576 * 5, 3, true);
        file_logger->set_level(spd::level::trace);
        spd::set_pattern("*** [%H:%M:%S:%e] [thread %t] %v ***");

        EnvironmentPlugin environmentPlugin("EnvironmentPlugin");
        r->installPlugin(&environmentPlugin);

        return CommandLineTestRunner::RunAllTests(ac, av);
    } catch (const spd::spdlog_ex& logException) {
        std::cerr << "creating log failed: " << logException.what() << std::endl;
        return 1;
    } catch(ARA::Exception& exception) {
        std::cout << "\nERROR: Tests failed with ARA::Exception: " << exception.getMessage() << "\n\n";
        return 1;
    } catch(const char* exceptionMessage) {
        std::cout << "\nERROR: Tests failed with const char* exception: " << exceptionMessage << "\n\n";
        return 1;
    }
}
