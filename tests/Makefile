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

CFLAGS += -std=c++11

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
INETMANET_LIB_DIR = ../inetmanet/src
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS) -L$(INETMANET_LIB_DIR) -linet

LD_LIB_PATH=$(OMNETPP_LIB_DIR):$(INETMANET_LIB_DIR)

# Include path and compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CPPUTEST_HOME = ../extern/cpputest
INCLUDE_PATH= -I../include -I../include/omnetpp -I./ -I$(OMNETPP_INCL_DIR) -I$(CPPUTEST_HOME)/include
LINKFLAGS= $(OMNETPP_LIBS) -L$(CPPUTEST_HOME)/lib -lCppUTest

# Enable CppUTest memory leak detection
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
#CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h

# Output directory ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = ../out/$(CONFIGNAME)
LIBARA_BIN = $(PROJECT_OUTPUT_DIR)/src/libara.so
TESTS_OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/tests
COMPILED_OMNETPP_ARA = $(shell find $(PROJECT_OUTPUT_DIR)/omnetpp -type f -name '*.o')

# Test sources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_TEST_SRC = $(shell find ./libara -type f -name '*Test.cpp')
LIBARA_TESTS_BIN = $(shell echo $(LIBARA_TEST_SRC) | sed 's/[a-zA-Z0-9./]* /&\n'/g | sed 's%\.\(.*\).cpp%$(TESTS_OUTPUT_DIR)\1.o%g')
OMNETPP_ARA_TEST_SRC = $(shell find ./omnetpp -type f -name '*Test.cpp')
OMNETPP_ARA_TESTS_BIN = $(shell echo $(OMNETPP_ARA_TESTS_BIN) | sed 's/[a-zA-Z0-9./]* /&\n'/g | sed 's%\.\(.*\).cpp%$(TESTS_OUTPUT_DIR)\1.o%g')
TESTAPI_SOURCES = $(shell find ./testAPI/ -name tests -prune -o -type f -name '*.cpp' -print)
TESTAPI_SOURCES += ./TestRunner.cpp
TESTAPI_BIN = $(shell echo $(TESTAPI_SOURCES) | sed 's/[a-zA-Z0-9./]* /&\n'/g | sed 's%\.\(.*\).cpp%$(TESTS_OUTPUT_DIR)\1.o%g')
TEST_EXECUTABLE = runAllTests

# Build targets~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
runSingleTest: $(TESTAPI_BIN) $(LIBARA_TESTS_BIN)
	@echo "Building executable test suit (./tests/$(TEST_EXECUTABLE))" 
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) -o $(TEST_EXECUTABLE) $(LIBARA_BIN) $(LIBARA_TESTS_BIN) $(TESTAPI_BIN) $(LINKFLAGS)
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"
	@if LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE) -n testGetDeliverablePacket; then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi	

runTests: all
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"	
	@if LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

all: check_if_cpputest_is_installed  $(LIBARA_BIN) $(TESTAPI_BIN) $(LIBARA_TESTS_BIN)
	@echo "Building executable test suit (./tests/$(TEST_EXECUTABLE))" 
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) -L$(PROJECT_OUTPUT_DIR)/src -lara $(LIBARA_TESTS_BIN) $(TESTAPI_BIN) $(LINKFLAGS) -o $(TEST_EXECUTABLE)

testsim: check_if_cpputest_is_installed  $(LIBARA_BIN) $(COMPILED_OMNETPP_ARA) $(TESTAPI_BIN) $(OMNETPP_ARA_TEST_BIN)
	@echo "Building executable test suit (./tests/$(TEST_EXECUTABLE))" 
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) -L$(PROJECT_OUTPUT_DIR)/src -lara $(COMPILED_OMNETPP_ARA) $(OMNETPP_ARA_TEST_BIN)  $(TESTAPI_BIN) $(LINKFLAGS) -o $(TEST_EXECUTABLE) 

.cpp.o: check_if_cpputest_is_installed
	@$(MKPATH) $(dir $(TESTS_OUTPUT_DIR)/$@)
	@if [[ $< == *Test.cpp ]]; then \
		nrOfTests=$$(grep -c "TEST(" $<); \
		echo "Compiling $< ($$nrOfTests tests)"; \
	else  \
		echo "Compiling $<"; \
	fi
	$(CXX) $(CFLAGS) -c $(INCLUDE_PATH) $< -o $@

clean:
	rm -f $(TEST_EXECUTABLE)
	rm -f $(LIBARA_TESTS_BIN) $(TESTAPI_BIN)

check_if_cpputest_is_installed:
	@if ! [ -d "$(CPPUTEST_HOME)" ]; then \
	echo >&2; \
	echo '======================================================================='>&2; \
	echo 'ERROR: It seems like CppUTest is not correctly installed. '>&2; \
	echo '       Please run make installCppUTest and try again'>&2; \
	echo '======================================================================='>&2; \
	echo>&2; \
	exit 1; \
	fi
	
installCppUTest:
	@echo -e "\n~~~ INSTALLING CppUTest FRAMEWORK FROM SOURCE ~~~~~~~~~~~~~~~~~~~\n"
	@if ! [ -d "../extern" ]; then \
	mkdir "../extern"; \
	fi
	git clone git://github.com/FGrosse/cpputest.git $(CPPUTEST_HOME)
	cd $(CPPUTEST_HOME) && $(MAKE)
	@echo; \
	echo '======================================================================='; \
	echo 'CppUTest framework has been installed successfully in ./extern/cpputest'; \
	echo '======================================================================='; \
	echo; \

