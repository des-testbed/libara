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
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

# Include path and compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CPPUTEST_HOME = ../extern/cpputest
INCLUDE_PATH= -I../include -I./ -I$(OMNETPP_INCL_DIR) -I$(CPPUTEST_HOME)/include
LINKFLAGS= $(OMNETPP_LIBS) -L$(CPPUTEST_HOME)/lib -lCppUTest

# Enable CppUTest memory leak detection
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
#CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h

# Output directory ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = ../out
SRC_OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/src
TESTS_OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/tests

# Test sources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
COMPILED_SRC = $(shell find $(SRC_OUTPUT_DIR) -type f -name '*.o')
TEST_SOURCES = $(shell find ./ -type f -name '*Test.cpp' -o -name 'Test*.cpp')
COMPILED_TESTS = $(TEST_SOURCES:.cpp=.o)
TESTAPI_SOURCES = $(shell find ./testAPI/ -name tests -prune -o -type f -name '*.cpp' -print)
COMPILED_TESTAPI = $(TESTAPI_SOURCES:.cpp=.o)
TEST_EXECUTABLE = runAllTests

# Build targets~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
runSingleTest: check_if_cpputest_is_installed $(COMPILED_TESTAPI) $(COMPILED_TESTS)
	@echo "Building executable test suit (./tests/$(TEST_EXECUTABLE))" 
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) -o $(TEST_EXECUTABLE) $(COMPILED_SRC) $(COMPILED_TESTS) $(COMPILED_TESTAPI) $(LINKFLAGS)
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"
	@if LD_LIBRARY_PATH=$(OMNETPP_LIB_DIR):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE) -n testClone; then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi	

searchMemoryLeak: check_if_cpputest_is_installed
	@echo "Building complete executable test suit in single compile (This might take some while))" 
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) -o $(TEST_EXECUTABLE) $(COMPILED_SRC) $(TEST_SOURCES) $(TESTAPI_SOURCES) $(LINKFLAGS)
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"
	@if LD_LIBRARY_PATH=$(OMNETPP_LIB_DIR):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

runTests: all
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"
	@if LD_LIBRARY_PATH=$(OMNETPP_LIB_DIR):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

all: check_if_cpputest_is_installed $(COMPILED_TESTAPI) $(COMPILED_TESTS) $(TEST_EXECUTABLE)	
	@echo 'All tests have been compiled'	

$(TEST_EXECUTABLE): $(COMPILED_TESTAPI) $(COMPILED_TESTS)
	@echo "Building executable test suit (./tests/$(TEST_EXECUTABLE))" 
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) -o $(TEST_EXECUTABLE) $(COMPILED_SRC) $(COMPILED_TESTS) $(COMPILED_TESTAPI) $(LINKFLAGS)	

.cpp.o:
	@if [[ $< == *Test.cpp ]]; then \
		nrOfTests=$$(grep -c "TEST(" $<); \
		echo "Compiling $< ($$nrOfTests tests)"; \
	else  \
		echo "Compiling $<"; \
	fi
	@$(CXX) $(CFLAGS) -c $(INCLUDE_PATH) $< -o $@

clean:
	rm -f $(TEST_EXECUTABLE) $(COMPILED_TESTS) $(COMPILED_TESTAPI)

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

