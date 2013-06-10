# Load OMNeT++ configuration (if wanted) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ifneq ("$(NO_OMNET)", "TRUE")
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
else
    $(info Running without OMNeT++ support)
endif

# Make "debug" the default mode
ifndef MODE
MODE = debug
endif

# TODO we might want to make this more generic (i.e. enable compiling with gcc)
# A simple ifndef does not work because this will always be set by the OMNeT+ makefile.inc to gcc
#ifndef CXX
CXX = clang
#endif

# Configname determines where (in which subdirectory of out/)
# makemake-generated makefiles create object files and other
# build artifacts.
CONFIGNAME = $(CXX)-$(MODE)

ARA_MAJOR_VERSION = 1
ARA_MINOR_VERSION = 0
ARA_TARGET_NAME = ARA
ARA_LIB_NAME = lib$(ARA_TARGET_NAME).so
ARA_SONAME = $(ARA_LIB_NAME).$(ARA_MAJOR_VERSION)

# Various tools and options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ifeq ($(MODE),debug)
    CFLAGS=$(CFLAGS_DEBUG)
    D=d
else
    CFLAGS=$(CFLAGS_RELEASE)
    D=
endif

CFLAGS_DEBUG = -g -Wall
CFLAGS_RELEASE = -O2 -DNDEBUG=1

MKPATH = mkdir -p

ifeq ("$(NO_OMNET)", "TRUE")
    LD_LIBRARY_PATH=LD_LIBRARY_PATH=./$(LIBARA_SRC_FOLDER):$(shell echo $$LD_LIBRARY_PATH)
else
    LD_LIBRARY_PATH=LD_LIBRARY_PATH=./$(LIBARA_SRC_FOLDER):$(OMNETPP_LIB_DIR):./$(INETMANET_SRC_FOLDER):$(shell echo $$LD_LIBRARY_PATH)
endif

# Folders ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_SRC_FOLDER = src
TESTS_FOLDER = tests
OMNETARA_SRC_FOLDER = omnetpp
TESTBEDARA_SRC_FOLDER = testbed
INETMANET_FOLDER = inetmanet
OUTPUT_DIR = out/$(CONFIGNAME)
INCLUDE_DIR = include

# libARA files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_SRC = $(shell find $(LIBARA_SRC_FOLDER)/ -type f -name '*.cpp')
LIBARA_O = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(LIBARA_SRC)))
LIBARA_DEPENDENCIES = $(LIBARA_O:.o=.d)

# omnetARA files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OMNETARA_MESSAGE_FILES = $(shell find $(OMNETARA_SRC_FOLDER)/ -type f -name '*.msg')
OMNETARA_MESSAGE_SRC = $(OMNETARA_MESSAGE_FILES:.msg=_m.cpp)
OMNETARA_MESSAGE_HEADERS = $(OMNETARA_MESSAGE_FILES:.msg=_m.h)
OMNETARA_SRC = $(shell find $(OMNETARA_SRC_FOLDER)/ -type f -name '*.cpp' -and -not -name '*_m.cpp')
OMNETARA_SRC += $(OMNETARA_MESSAGE_SRC)
OMNETARA_O = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(OMNETARA_SRC)))
OMNETARA_DEPENDENCIES = $(OMNETARA_O:.o=.d)
OMNETARA_EXECUTABLE_NAME = ara-sim
OMNETARA_EXECUTABLE = $(OUTPUT_DIR)/$(OMNETARA_SRC_FOLDER)/$(OMNETARA_EXECUTABLE_NAME)

# testbedARA files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TESTBEDARA_MAIN = ara_main
TESTBEDARA_SRC_MAIN = $(shell find $(TESTBEDARA_SRC_FOLDER)/ -type f -name '$(TESTBEDARA_MAIN)')
TESTBEDARA_SRC_MAIN_O = $(shell find $(TESTBEDARA_SRC_FOLDER)/ -type f -name '$(TESTBEDARA_MAIN).o')
TESTBEDARA_SRC = $(shell find $(TESTBEDARA_SRC_FOLDER)/ -type f -name '*.cpp' -a -not -name '$(TESTBEDARA_MAIN).cpp')
TESTBEDARA_O = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(TESTBEDARA_SRC)))
TESTBEDARA_DEPENDENCIES = $(TESTBEDARA_O:.o=.d)
TESTBEDARA_EXECUTABLE_NAME = ara
TESTBEDARA_EXECUTABLE = $(OUTPUT_DIR)/$(TESTBEDARA_SRC_FOLDER)/$(TESTBEDARA_EXECUTABLE_NAME)
TESTBEDARA_LINKFLAGS = $(LINKFLAGS) -ldessert -lcli -lstdc++ -lm

# Tests files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_TESTS_SRC = $(shell find $(TESTS_FOLDER)/libara -type f -name '*Test.cpp')
LIBARA_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(LIBARA_TESTS_SRC)))

OMNETPP_ARA_TESTS_SRC = $(shell find $(TESTS_FOLDER)/omnetpp -type f -name '*Test.cpp')
OMNETPP_ARA_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(OMNETPP_ARA_TESTS_SRC)))

TESTBED_ARA_TESTS_SRC = $(shell find $(TESTS_FOLDER)/testbed -type f -name '*Test.cpp')
TESTBED_ARA_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(TESTBED_ARA_TESTS_SRC)))

TESTAPI_SRC = $(shell find $(TESTS_FOLDER)/testAPI/ -name tests -prune -o -type f -name '*.cpp' -print)
TESTAPI_SRC += $(TESTS_FOLDER)/TestRunner.cpp
TESTAPI_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(TESTAPI_SRC)))

TESTAPI_TESTS_SRC = $(shell find $(TESTS_FOLDER)/testAPI/tests -type f -name '*Test.cpp')
TESTAPI_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(TESTAPI_TESTS_SRC)))

ALL_TEST_BINARIES = $(LIBARA_TESTS_BIN) $(OMNETPP_ARA_TESTS_BIN) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN)
TESTS_DEPENDENCIES = $(ALL_TEST_BINARIES:.o=.d)

TEST_EXECUTABLE = runAllTests
LIBARA_TEST_EXECUTABLE = runLibAraTests
OMNETPP_ARA_TEST_EXECUTABLE = runOmnetAraTests
TESTBED_ARA_TEST_EXECUTABLE = runTestbedAraTests

# Compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS += -std=c++11 -fPIC
ifeq ("$(NO_OMNET)", "TRUE")
    INCLUDE_PATH = -I$(INCLUDE_DIR)
else
    INCLUDE_PATH += -I. \
                    -I$(INCLUDE_DIR)
                    -I$(INCLUDE_DIR)/omnetpp
                    -I$(INCLUDE_DIR)/testbed
                    -I$(INCLUDE_DIR)/inetmanet
                    -I$(OMNETPP_INCL_DIR)
                    -I$(INETMANET_SRC_FOLDER)
endif
INCLUDE_PATH += -I$(TESTS_FOLDER) -I$(CPPUTEST_BASE_DIR)/include
LINK_TO_LIB_ARA = -L$(LIBARA_SRC_FOLDER) -l$(ARA_TARGET_NAME)

# Tests configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CPPUTEST_BASE_DIR = $(TESTS_FOLDER)/cpputest
CPPUTEST_LIB_DIR = $(CPPUTEST_BASE_DIR)/lib
CPPUTEST_LIB = $(CPPUTEST_LIB_DIR)/libCppUTest.a
TESTS_CFLAGS = $(CFLAGS) -include $(CPPUTEST_BASE_DIR)/include/CppUTest/MemoryLeakDetectorNewMacros.h
TESTS_LINKFLAGS = $(LINK_TO_LIB_ARA) -L$(CPPUTEST_LIB_DIR) -lCppUTest -lm -lstdc++

# Simulation kernel and user interface libraries ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
INETMANET_SRC_FOLDER = $(INETMANET_FOLDER)/src
INETMANET_LIB = $(INETMANET_SRC_FOLDER)/libinet.so
INETMANET_FOLDERS_INCLUDE = $(addprefix -I, $(shell find $(INETMANET_SRC_FOLDER) -type f -name "*.h" | sed 's%\(.*\)/.*%\1%' | sort -u))
OMNETPP_LINKFLAGS = $(LINKFLAGS) $(OMNETPP_LIBS) -L$(INETMANET_SRC_FOLDER) -linet -lm
OMNETPP_LIBS = -L"$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)" -L"$(OMNETPP_LIB_DIR)" $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
USERIF_LIBS = $(AS_NEEDED_OFF) -loppcmdenv$D -loppenvir$D $(AS_NEEDED_OFF) -loppenvir$D -lopplayout$D
ifneq ("$(HEADLESS)", "TRUE")
    USERIF_LIBS += -lopptkenv$D
endif

# Build targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: all
all: libARA omnetARA

.PHONY: libARA
libARA: $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME)

#
# Build the ARA library (libARA)
#
$(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME): $(LIBARA_O)
	@echo "Linking $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME)"
	@$(CXX) -shared $(CFLAGS) $(INCLUDE_PATH) $(LINKFLAGS) $(LIBARA_O) \
		-o $(OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION)
	@cd $(LIBARA_SRC_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION) $(ARA_LIB_NAME)

# pull in dependeny file (if existing)
-include $(LIBARA_DEPENDENCIES)
-include $(OMNETARA_DEPENDENCIES)
-include $(TESTS_DEPENDENCIES)
-include $(TESTBEDARA_DEPENDENCIES)

# OMNeTARA target ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.PHONY: omnetARA
omnetARA: $(OMNETARA_EXECUTABLE)

#
# Build the OMNeT++ simulations
#
$(OMNETARA_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) $(INETMANET_LIB) $(OMNETARA_O)
	@echo "Linking $(OMNETARA_SRC_FOLDER)/$(OMNETARA_EXECUTABLE_NAME)"
	@$(CXX) $(OMNETARA_O) -o $(OMNETARA_EXECUTABLE) $(LINK_TO_LIB_ARA) $(OMNETPP_LINKFLAGS) -loppmain$D 
	@cd $(OMNETARA_SRC_FOLDER) && ln -s -f ../$(OMNETARA_EXECUTABLE) $(OMNETARA_EXECUTABLE_NAME)
	@echo "You can start the simulation now with the individual run scripts in the simulations folder"

#
# Creates the *.cpp files for all *.msg files
#
.SECONDARY: $(OMNETARA_MESSAGE_SRC)
%_m.cpp: %.msg
	@echo "Compiling $<"
	@opp_msgc -s "_m.cpp" $<

.SECONDARY: $(OMNETARA_MESSAGE_HEADERS)
%_m.h: %_m.cpp
	

#
# Builds omnetpp *.cpp files. This is separate from the default %.o target because it uses other includes
#
$(OUTPUT_DIR)/$(OMNETARA_SRC_FOLDER)/%.o: $(OMNETARA_SRC_FOLDER)/%.cpp $(INETMANET_LIB) $(OMNETARA_MESSAGE_HEADERS)
	@$(MKPATH) $(dir $@)
	@echo "Compiling $< (belongs to omnetARA)";
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) $(INETMANET_FOLDERS_INCLUDE) -c $< -o $@
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) $(INETMANET_FOLDERS_INCLUDE) -MM -MT $@ $< > $(OUTPUT_DIR)/$(OMNETARA_SRC_FOLDER)/$*.d

#
# Builds omnetpp *Test.cpp files. This is separate from the default %.o target because it uses other includes
#
$(OUTPUT_DIR)/$(TESTS_FOLDER)/$(OMNETARA_SRC_FOLDER)/%.o: $(TESTS_FOLDER)/$(OMNETARA_SRC_FOLDER)/%.cpp $(INETMANET_LIB)
	@$(MKPATH) $(dir $@)
	@echo "Compiling $< (belongs to omnetARA)";
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) $(INETMANET_FOLDERS_INCLUDE) -c $< -o $@
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) $(INETMANET_FOLDERS_INCLUDE) -MM -MT $@ $< > $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(OMNETARA_SRC_FOLDER)/$*.d

#
# Build the inetmanet simulation library
#
$(INETMANET_LIB): $(INETMANET_FOLDER)/.git
	@echo -e "\n~~~ BUILDING INET/MANET FRAMEWORK ~~~\n"
	@cd $(INETMANET_FOLDER) && $(MAKE) makefiles
	@cd $(INETMANET_FOLDER) && $(MAKE)

#
# Checkout the inetmanet git submodule
#
$(INETMANET_FOLDER)/.git:
	@echo -e "\n~~~ INITIALIZING INET/MANET SUBMODULE ~~~\n"
	git submodule init $(INETMANET_FOLDER)
	git submodule update $(INETMANET_FOLDER)


# testbedARA target ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: testbedARA
testbedARA: $(TESTBEDARA_EXECUTABLE)

#
# Build the DES-Testbed version of libARA
#
$(TESTBEDARA_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) $(TESTBEDARA_O) $(TESTBEDARA_SRC_MAIN_O)
	@echo "Linking $(TESTBEDARA_SRC_FOLDER)/$(TESTBEDARA_EXECUTABLE_NAME)"
	@$(CXX) $(TESTBEDARA_O) -o $(TESTBEDARA_EXECUTABLE) $(TESTBEDARA_LINKFLAGS) $(LINK_TO_LIB_ARA)
	@cd $(TESTBEDARA_SRC_FOLDER) && ln -s -f ../$(TESTBEDARA_EXECUTABLE) $(TESTBEDARA_EXECUTABLE_NAME)


# Test targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: test
test: testbedTests

.PHONY: allTests
allTests: $(TESTS_FOLDER)/$(TEST_EXECUTABLE) $(OMNETARA_EXECUTABLE)
	@echo -e "\n~~~ RUNNING complete TEST SUIT ~~~~~~"
	@if $(LD_LIBRARY_PATH) $(TESTS_FOLDER)/$(TEST_EXECUTABLE); then \
 		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
	else \
		echo -e "~~~ ERROR WHILE RUNNING TEST ~~~~~~~~\n"; \
		exit 1; \
	fi

.PHONY: libAraTests
libAraTests: $(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING libARA TEST SUIT ~~~~~~~~"	
	@if $(LD_LIBRARY_PATH) $(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
	else \
		echo -e "~~~ ERROR WHILE RUNNING TEST ~~~~~~~~\n"; \
		exit 1; \
	fi

.PHONY: omnetTests
omnetTests: $(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING OMNET ARA TEST SUIT ~~~~~"	
	@if $(LD_LIBRARY_PATH) $(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
	else \
		echo -e "~~~ ERROR WHILE RUNNING TEST ~~~~~~~~\n"; \
		exit 1; \
	fi

.PHONY: testbedTests
testbedTests: $(TESTS_FOLDER)/$(TESTBED_ARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING Testbed ARA TEST SUIT ~~~~~"	
	@if $(LD_LIBRARY_PATH) $(TESTS_FOLDER)/$(TESTBED_ARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
	else \
		echo -e "~~~ ERROR WHILE RUNNING TEST ~~~~~~~~\n"; \
		exit 1; \
	fi

#
# Builds all available tests (libara and omnet ARA)
#
$(TESTS_FOLDER)/$(TEST_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) \
                                    $(OMNETARA_O) \
                                    $(CPPUTEST_LIB) \
                                    $(ALL_TEST_BINARIES)
	@echo "Building the comlete test suit ($@)"
	@$(CXX) $(TESTS_CFLAGS) $(OMNETARA_O) $(ALL_TEST_BINARIES) \
                $(INCLUDE_PATH) $(TESTS_LINKFLAGS) $(OMNETPP_LINKFLAGS) \
                -o $(OUTPUT_DIR)/$@
	@cd $(TESTS_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$@ $(TEST_EXECUTABLE)

#
# Tests only the libara sources
#
$(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) \
                                           $(CPPUTEST_LIB) \
                                           $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN)
	@echo "Building the libara test suit ($@)"
	@$(CXX) $(TESTS_CFLAGS) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN) \
                $(INCLUDE_PATH) $(TESTS_LINKFLAGS) \
                -o $(OUTPUT_DIR)/$@
	@cd $(TESTS_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$@ $(LIBARA_TEST_EXECUTABLE)

#
# Tests only the omnetpp ARA sources
#
$(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) \
                                                $(CPPUTEST_LIB) \
                                                $(OMNETARA_O) \
                                                $(TESTAPI_BIN) $(OMNETPP_ARA_TESTS_BIN)
	@echo "Building the omnetara test suit ($@)"
	@$(CXX) $(TESTS_CFLAGS) $(TESTAPI_BIN) $(OMNETPP_ARA_TESTS_BIN) $(OMNETARA_O) \
                $(INCLUDE_PATH) $(TESTS_LINKFLAGS) $(OMNETPP_LINKFLAGS) \
                -o $(OUTPUT_DIR)/$@
	@cd $(TESTS_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$@ $(OMNETPP_ARA_TEST_EXECUTABLE)
	
#
# Tests only the omnetpp ARA sources
#
$(TESTS_FOLDER)/$(TESTBED_ARA_TEST_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) \
                                                $(CPPUTEST_LIB) \
                                                $(TESTBEDARA_O) \
                                                $(TESTAPI_BIN) $(TESTBED_ARA_TESTS_BIN)
	@echo "Building the testbed test suit ($@)"
	@$(CXX) $(TESTS_CFLAGS) $(TESTAPI_BIN) $(TESTBED_ARA_TESTS_BIN) $(TESTBEDARA_O) \
                $(INCLUDE_PATH) $(TESTS_LINKFLAGS) $(TESTBEDARA_LINKFLAGS) \
                -o $(OUTPUT_DIR)/$@
	@cd $(TESTS_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$@ $(TESTBED_ARA_TEST_EXECUTABLE)

#
# Builds the CppUTest Framework
#
$(CPPUTEST_LIB): $(CPPUTEST_BASE_DIR)/.git
	@echo -e "\n~~~ BUILDING CPPUTEST FRAMEWORK ~~~~~\n"
	@cd $(CPPUTEST_BASE_DIR) && $(MAKE)

$(CPPUTEST_BASE_DIR)/.git:
	@echo -e "\n~~~ INITIALIZING CPPUTEST SUBMODULE ~\n"
	@git submodule init $(CPPUTEST_BASE_DIR)
	@git submodule update $(CPPUTEST_BASE_DIR)

# Other targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#
# Builds most of the cpp files (omnetARA & omnetARATest cpps are handled above)
#
$(OUTPUT_DIR)/%.o: %.cpp
	@$(MKPATH) $(dir $@)
	@echo "Compiling $<";
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) $(INETMANET_FOLDERS_INCLUDE) -c $< -o $@
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) $(INETMANET_FOLDERS_INCLUDE) -MM -MT $@ $< > $(OUTPUT_DIR)/$*.d;

#
# Clean up only the dependencies
#
.PHONY: cleandep
cleandep:
	@rm -f $(shell find $(OUTPUT_DIR) -type f -name '*.d')

#
# Clean up (delete) all generated files
#
.PHONY: clean
clean: cleandep
	@echo "Cleaning libARA.."
	@rm -f $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) $(OUTPUT_DIR)/$(ARA_LIB_NAME).*
	@rm -f $(LIBARA_O)
	@echo "Cleaning omnetARA.."
	@rm -f $(OMNETARA_SRC_FOLDER)/$(OMNETARA_EXECUTABLE_NAME) $(OMNETARA_EXECUTABLE)
	@rm -f $(OMNETARA_O)
	@rm -f $(OMNETARA_MESSAGE_HEADERS) $(OMNETARA_MESSAGE_SRC)
	@echo "Cleaning testbedARA.."
	@rm -f $(TESTBEDARA_SRC_FOLDER)/$(TESTBEDARA_EXECUTABLE_NAME) $(TESTBEDARA_EXECUTABLE)
	@rm -f $(TESTBEDARA_O)
	@rm -f $(TESTBEDARA_SRC_O)
	@echo "Cleaning tests.."
	@rm -f $(ALL_TEST_BINARIES)
	@rm -f $(TESTS_FOLDER)/$(TEST_EXECUTABLE) $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(TEST_EXECUTABLE)
	@rm -f $(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE) $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE)
	@rm -f $(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE) $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	@rm -f $(TESTS_FOLDER)/$(TESTBED_ARA_TEST_EXECUTABLE) $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(TESTBED_ARA_TEST_EXECUTABLE)

.PHONY: clobber
clobber: clean
	@echo "Deleting submodule $(CPPUTEST_BASE_DIR)"
	@rm -R -f $(CPPUTEST_BASE_DIR)
	@echo "Deleting submodule $(INETMANET_FOLDER)"
	@rm -R -f $(INETMANET_FOLDER)
	@echo "Deleting out/$(CONFIGNAME)"
	@rm -R -f out/$(CONFIGNAME)

#
# Build everything, run the tests and then push it to upstream
#
.PHONY: release
release: allTests
	@echo -e "\n~~~ PUSHING TO GIT ORIGIN ~~~~~~~~~~~\n"
	git push

#
# Generate the documentation using doxygen
#
.PHONY: doc
doc: all
	@if type doxygen >/dev/null 2>&1; then \
	if type dot >/dev/null 2>&1; then \
	echo; \
	echo '======================================================================='; \
	echo 'building doxygen-based documentation in doc/html'; \
	echo '======================================================================='; \
	echo; \
	doxygen doc/doxygen/config.doxyfile; \
	else \
	echo '======================================================================='; \
	echo 'please check if dot (graphviz) is installed'; \
	echo '======================================================================='; \
   	exit 1; \
	fi \
	else \
	echo '======================================================================='; \
	echo 'please check if doxygen is installed'; \
	echo '======================================================================='; \
	exit 1; \
	fi
     
