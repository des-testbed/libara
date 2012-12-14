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
INCLUDE_PATH= -I../include -I./ -I$(OMNETPP_INCL_DIR)
LINKFLAGS= -lUnitTest++ $(OMNETPP_LIBS)

# Output directory ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = ../out
SRC_OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/src

# Test sources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
COMPILED_SRC = $(shell find $(SRC_OUTPUT_DIR) -type f -name '*.o')
TEST_SOURCES = $(shell find ./ -name testAPI -prune -o -type f -name '*.cpp' -print)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TESTAPI_TESTS = $(shell find ./testAPI/tests/ -type f -name '*.cpp')
TESTAPI_OBJECTS = $(shell find ./testAPI/ -type f -name '*.cpp')
TEST_EXECUTABLE = runAllTests

# Build targets~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
runTests: all
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"
	@if LD_LIBRARY_PATH=$(OMNETPP_LIB_DIR):$$LD_LIBRARY_PATH ./runAllTests; then \
		echo -e "\n~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

all: check_if_unittest_is_installed $(TEST_SOURCES) $(TEST_EXECUTABLE)	

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(TESTAPI_TESTS)
	@echo "Building executable test suit.." 
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) -o $@ $(COMPILED_SRC) $(TEST_OBJECTS) $(TESTAPI_OBJECTS) $(LINKFLAGS)	

.cpp.o:
	@echo "Compiling $<.."
	@$(CXX) $(CFLAGS) -c $(INCLUDE_PATH) $< -o $@

clean:
	rm -f $(TEST_OBJECTS) $(TESTAPI_OBJECTS:.cpp=.o) TestResults.xml runAllTests
	
check_if_unittest_is_installed:
	@if ! [ -e "/lib/libUnitTest++.a" ]; then \
	echo >&2; \
	echo '======================================================================='>&2; \
	echo 'ERROR: It seems like UnitTest++ library is not installed on this system'>&2; \
	echo '======================================================================='>&2; \
	echo>&2; \
	exit 1; \
	fi