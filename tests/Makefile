include ../Makefile.inc

ifneq ("$(NO_OMNET)", "TRUE")
# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)
endif

# Include path and compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS += -std=c++11
INCLUDE_PATH = -I./ -I../include -I$(CPPUTEST_BASE_DIR)/include
LINKFLAGS = -L$(ARA_LIB_DIR) -l$(ARA_TARGET_NAME) -L$(CPPUTEST_LIB_DIR) -lCppUTest
LD_LIB_PATH = $(ARA_LIB_DIR)

# libARA stuff
ARA_LIB_DIR = ../src
ARA_LIB = $(ARA_LIB_DIR)/$(ARA_LIB_NAME)

# CppUTest options
CPPUTEST_BASE_DIR = ./cpputest
CPPUTEST_LIB_DIR = $(CPPUTEST_BASE_DIR)/lib
CPPUTEST_LIB = $(CPPUTEST_LIB_DIR)/libCppUTest.a
CFLAGS += -include $(CPPUTEST_BASE_DIR)/include/CppUTest/MemoryLeakDetectorNewMacros.h

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
INETMANET_LIB_DIR = ../inetmanet/src
OMNETPP_INCLUDE_PATH = $(INCLUDE_PATH) -I../include/omnetpp -I$(OMNETPP_INCL_DIR)
OMNETPP_LINKFLAGS = $(LINKFLAGS) $(OMNETPP_LIBS) -L$(INETMANET_LIB_DIR) -linet
OMNETPP_LIB_PATH = $(LD_LIB_PATH):$(OMNETPP_LIB_DIR):$(INETMANET_LIB_DIR)

# Output directory and files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = ../out/$(CONFIGNAME)
TESTS_OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/tests
COMPILED_OMNETPP_ARA = $(shell find $(PROJECT_OUTPUT_DIR)/omnetpp -type f -name '*.o')

TEST_EXECUTABLE = runAllTests
LIBARA_TEST_EXECUTABLE = libAraTests
OMNETPP_ARA_TEST_EXECUTABLE = omnetAraTests

# Test sources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_TEST_SRC = $(shell find ./libara -type f -name '*Test.cpp')
LIBARA_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(TESTS_OUTPUT_DIR)/, $(LIBARA_TEST_SRC)))

OMNETPP_ARA_TEST_SRC = $(shell find ./omnetpp -type f -name '*Test.cpp')
OMNETPP_ARA_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(TESTS_OUTPUT_DIR)/, $(OMNETPP_ARA_TEST_SRC)))

TESTAPI_SOURCES = $(shell find ./testAPI/ -name tests -prune -o -type f -name '*.cpp' -print)
TESTAPI_SOURCES += ./TestRunner.cpp
TESTAPI_BIN = $(subst .cpp,.o, $(addprefix $(TESTS_OUTPUT_DIR)/, $(TESTAPI_SOURCES)))

TESTAPI_TESTS_SRC = $(shell find ./testAPI/tests -type f -name '*Test.cpp')
TESTAPI_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(TESTS_OUTPUT_DIR)/, $(TESTAPI_TESTS_SRC)))

# Build targets~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
runTests: all
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"	
	@if LD_LIBRARY_PATH=$(OMNETPP_LIB_PATH):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

runLibAraTests: $(LIBARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING libARA TEST SUIT ~~~~~~~~\n"	
	@if LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH ./$(LIBARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

runOmnetAraTests: $(OMNETPP_ARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING OMNET ARA TEST SUIT ~~~~~\n"	
	@if LD_LIBRARY_PATH=$(OMNETPP_LIB_PATH):$$LD_LIBRARY_PATH ./$(OMNETPP_ARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

all: $(TEST_EXECUTABLE)

#
# Builds all available tests (libara and omnet ARA)
#
$(TEST_EXECUTABLE): $(ARA_LIB) $(CPPUTEST_LIB) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN) $(OMNETPP_ARA_TEST_BIN)
	@echo "Building the comlete test suit (./$(TEST_EXECUTABLE))"
	@$(CXX) $(CFLAGS) $(OMNETPP_INCLUDE_PATH) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN) $(OMNETPP_ARA_TEST_BIN) $(OMNETPP_LINKFLAGS) -o $(TESTS_OUTPUT_DIR)/$(TEST_EXECUTABLE)
	@ln -s -f $(TESTS_OUTPUT_DIR)/$(TEST_EXECUTABLE) $(TEST_EXECUTABLE)

#
# Tests only the libara sources
#
$(LIBARA_TEST_EXECUTABLE): $(ARA_LIB) $(CPPUTEST_LIB) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN)
	@echo "Building the libara test suit (./$(LIBARA_TEST_EXECUTABLE))"
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN) $(LINKFLAGS) -o $(TESTS_OUTPUT_DIR)/$(LIBARA_TEST_EXECUTABLE)
	@ln -s -f $(TESTS_OUTPUT_DIR)/$(LIBARA_TEST_EXECUTABLE) $(LIBARA_TEST_EXECUTABLE)

#
# Tests only the omnetpp ARA sources
#
$(OMNETPP_ARA_TEST_EXECUTABLE): $(ARA_LIB) $(CPPUTEST_LIB) $(TESTAPI_BIN) $(OMNETPP_ARA_TEST_BIN)
	@echo "Building the libara test suit (./$(OMNETPP_ARA_TEST_EXECUTABLE))"
	@$(CXX) $(CFLAGS) $(OMNETPP_INCLUDE_PATH) $(TESTAPI_BIN) $(OMNETPP_ARA_TEST_BIN) $(OMNETPP_LINKFLAGS) -o $(TESTS_OUTPUT_DIR)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	@ln -s -f $(TESTS_OUTPUT_DIR)/$(OMNETPP_ARA_TEST_EXECUTABLE) $(OMNETPP_ARA_TEST_EXECUTABLE)

#
# Builds all cpp files in the test folder and its subfolders (recursive)
#
$(TESTS_OUTPUT_DIR)/%.o:	
	@$(MKPATH) $(dir $@)
	@if [[ $* == *Test ]]; then \
		nrOfTests=$$(grep -c "TEST(" $*.cpp); \
		echo "Compiling $*.cpp ($$nrOfTests tests)"; \
	else  \
		echo "Compiling $*.cpp"; \
	fi
	@$(CXX) $(CFLAGS) -c $(INCLUDE_PATH) $*.cpp -o $@

#
# Builds the CppUTest Framework
#
$(CPPUTEST_LIB): $(CPPUTEST_BASE_DIR)/.git
	@echo -e "\n~~~ BUILDING CPPUTEST FRAMEWORK ~~~~~\n"
	@cd $(CPPUTEST_BASE_DIR) && $(MAKE)

$(CPPUTEST_BASE_DIR)/.git:
	@echo -e "\n~~~ INITIALIZING CPPUTEST SUBMODULE ~\n";
	@cd ../ && git submodule init tests/cpputest;
	@cd ../ && git submodule update tests/cpputest;

clean:
	rm -f $(TEST_EXECUTABLE) $(TESTS_OUTPUT_DIR)/$(TEST_EXECUTABLE)
	rm -f $(LIBARA_TEST_EXECUTABLE) $(TESTS_OUTPUT_DIR)/$(LIBARA_TEST_EXECUTABLE)
	rm -f $(OMNETPP_ARA_TEST_EXECUTABLE) $(TESTS_OUTPUT_DIR)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	rm -f $(TESTAPI_BIN)
	rm -f $(LIBARA_TESTS_BIN)
	rm -f $(OMNETPP_ARA_TESTS_BIN)

