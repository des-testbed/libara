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
ARA_LIB_DIR = ../src
INETMANET_LIB_DIR = ../inetmanet/src
CPPUTEST_LIB_DIR = ../extern/cpputest
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
LD_LIB_PATH=$(OMNETPP_LIB_DIR):$(INETMANET_LIB_DIR):$(ARA_LIB_DIR)

# Include path and compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INCLUDE_PATH= -I../include -I../include/omnetpp -I./ -I$(OMNETPP_INCL_DIR) -I$(CPPUTEST_LIB_DIR)/include
LINKFLAGS= $(OMNETPP_LIBS) -L$(ARA_LIB_DIR) -lara -L$(INETMANET_LIB_DIR) -linet -L$(CPPUTEST_LIB_DIR)/lib -lCppUTest

# Enable CppUTest memory leak detection
CFLAGS += -include $(CPPUTEST_LIB_DIR)/include/CppUTest/MemoryLeakDetectorNewMacros.h
#CFLAGS += -include $(CPPUTEST_LIB_DIR)/include/CppUTest/MemoryLeakDetectorMallocMacros.h

# Output directory and files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = ../out/$(CONFIGNAME)
LIB_ARA_BIN = $(PROJECT_OUTPUT_DIR)/src/libara.so
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

# Build targets~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
runTests: all
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"	
	@if LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

runLibAraTests: $(LIBARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING LIBARA TEST SUIT ~~~~~~~~\n"	
	@if LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH $(TESTS_OUTPUT_DIR)/$(LIBARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi

runOmnetAraTests: $(OMNETPP_ARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING OMNET ARA TEST SUIT ~~~~~\n"	
	@if LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH $(TESTS_OUTPUT_DIR)/$(OMNETPP_ARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi
    
runSingleTest: all
	@echo -e "\n~~~ RUNNING A SINGLE TEST ~~~~~~~~~~~\n"
	@if LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH ./$(TEST_EXECUTABLE) -n testGetDeliverablePacket; then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi	

all: $(TEST_EXECUTABLE)

# Builds all available tests (libara and omnet ARA)
$(TEST_EXECUTABLE): check_if_cpputest_is_installed $(LIB_ARA_BIN) $(TESTAPI_BIN) $(LIBARA_TESTS_BIN) $(OMNETPP_ARA_TEST_BIN)
	@echo "Building the comlete test suit (./$(TEST_EXECUTABLE))"
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(TESTAPI_BIN) $(LIBARA_TESTS_BIN) $(OMNETPP_ARA_TEST_BIN) $(LINKFLAGS) -o $(TESTS_OUTPUT_DIR)/$(TEST_EXECUTABLE)
	@ln -s $(TESTS_OUTPUT_DIR)/$(TEST_EXECUTABLE) $(TEST_EXECUTABLE)

# Tests only the libara sources
$(LIBARA_TEST_EXECUTABLE): check_if_cpputest_is_installed $(LIB_ARA_BIN) $(TESTAPI_BIN) $(LIBARA_TESTS_BIN)
	@echo "Building the libara test suit (./$(LIBARA_TEST_EXECUTABLE))"
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(TESTAPI_BIN) $(LIBARA_TESTS_BIN) $(LINKFLAGS) -o $(TESTS_OUTPUT_DIR)/$(LIBARA_TEST_EXECUTABLE)
	@ln -s $(TESTS_OUTPUT_DIR)/$(LIBARA_TEST_EXECUTABLE) $(LIBARA_TEST_EXECUTABLE)

# Tests only the omnetpp ARA sources
$(OMNETPP_ARA_TEST_EXECUTABLE): check_if_cpputest_is_installed $(LIB_ARA_BIN) $(TESTAPI_BIN) $(OMNETPP_ARA_TEST_BIN)
	@echo "Building the libara test suit (./$(OMNETPP_ARA_TEST_EXECUTABLE))"
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(TESTAPI_BIN) $(OMNETPP_ARA_TEST_BIN) $(LINKFLAGS) -o $(TESTS_OUTPUT_DIR)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	@ln -s $(TESTS_OUTPUT_DIR)/$(OMNETPP_ARA_TEST_EXECUTABLE) $(OMNETPP_ARA_TEST_EXECUTABLE)

# Builds all cpp files in the test folder and its subfolders (recursive)
$(TESTS_OUTPUT_DIR)/%.o:	
	@$(MKPATH) $(dir $@)
	@if [[ $* == *Test ]]; then \
		nrOfTests=$$(grep -c "TEST(" $*.cpp); \
		echo "Compiling $*.cpp ($$nrOfTests tests)"; \
	else  \
		echo "Compiling $*.cpp"; \
	fi
	@$(CXX) $(CFLAGS) -c $(INCLUDE_PATH) $*.cpp -o $@

clean:
	rm -f $(TEST_EXECUTABLE) $(TESTS_OUTPUT_DIR)/$(TEST_EXECUTABLE)
	rm -f $(LIBARA_TEST_EXECUTABLE) $(TESTS_OUTPUT_DIR)/$(LIBARA_TEST_EXECUTABLE)
	rm -f $(OMNETPP_ARA_TEST_EXECUTABLE) $(TESTS_OUTPUT_DIR)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	rm -f $(TESTAPI_BIN)
	rm -f $(LIBARA_TESTS_BIN)
	rm -f $(OMNETPP_ARA_TESTS_BIN)

check_if_cpputest_is_installed:
	@if ! [ -d "$(CPPUTEST_LIB_DIR)" ]; then \
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
	git clone git://github.com/FGrosse/cpputest.git $(CPPUTEST_LIB_DIR)
	cd $(CPPUTEST_LIB_DIR) && $(MAKE)
	@echo; \
	echo '======================================================================='; \
	echo 'CppUTest framework has been installed successfully in ./extern/cpputest'; \
	echo '======================================================================='; \
	echo; \

