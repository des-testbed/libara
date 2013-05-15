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
endif

# our makefile needs to be included after the omnet stuff (otherwise $(CXX) is overridden ~~~~~~~~~
include Makefile.inc
CONFIGNAME = $(CXX)-$(MODE)

# Source folder ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_SRC_FOLDER = src
OMNETARA_SRC_FOLDER = omnetpp
TESTS_FOLDER = tests
OUTPUT_DIR = out/$(CONFIGNAME)

# libARA files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_SRC = $(shell find $(LIBARA_SRC_FOLDER)/ -type f -name '*.cpp')
LIBARA_O = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(LIBARA_SRC)))
LIBARA_DEPENDENCIES = $(LIBARA_O:.o=.d)

# omnetARA files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OMNETARA_SRC = $(shell find $(OMNETARA_SRC_FOLDER)/ -type f -name '*.cpp')
OMNETARA_O = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(OMNETARA_SRC)))

# Tests files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_TESTS_SRC = $(shell find $(TESTS_FOLDER)/libara -type f -name '*Test.cpp')
LIBARA_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(LIBARA_TESTS_SRC)))

OMNETPP_ARA_TESTS_SRC = $(shell find $(TESTS_FOLDER)/omnetpp -type f -name '*Test.cpp')
OMNETPP_ARA_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(OMNETPP_ARA_TESTS_SRC)))

TESTAPI_SRC = $(shell find $(TESTS_FOLDER)/testAPI/ -name tests -prune -o -type f -name '*.cpp' -print)
TESTAPI_SRC += $(TESTS_FOLDER)/TestRunner.cpp
TESTAPI_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(TESTAPI_SRC)))

TESTAPI_TESTS_SRC = $(shell find $(TESTS_FOLDER)/testAPI/tests -type f -name '*Test.cpp')
TESTAPI_TESTS_BIN = $(subst .cpp,.o, $(addprefix $(OUTPUT_DIR)/, $(TESTAPI_TESTS_SRC)))

ALL_TEST_BINARIES = $(LIBARA_TESTS_BIN) $(OMNETPP_ARA_TESTS_BIN) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN)
TESTS_DEPENDENCIES = $(ALL_TEST_BINARIES:.o=.d)

TEST_EXECUTABLE = runAllTests
LIBARA_TEST_EXECUTABLE = libAraTests
OMNETPP_ARA_TEST_EXECUTABLE = omnetAraTests

# Compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS += -std=c++11 -fPIC
INCLUDE_PATH = -Iinclude
INCLUDE_PATH += -I$(TESTS_FOLDER) -I$(CPPUTEST_BASE_DIR)/include
ifneq ("$(NO_OMNET)", "TRUE")
    ADDITIONAL_INCLUDES = $(OMNETPP_INCLUDE_PATH)
endif

# Tests configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CPPUTEST_BASE_DIR = $(TESTS_FOLDER)/cpputest
CPPUTEST_LIB_DIR = $(CPPUTEST_BASE_DIR)/lib
CPPUTEST_LIB = $(CPPUTEST_LIB_DIR)/libCppUTest.a
TESTS_CFLAGS = $(CFLAGS) -include $(CPPUTEST_BASE_DIR)/include/CppUTest/MemoryLeakDetectorNewMacros.h
TESTS_LINKFLAGS = -L$(LIBARA_SRC_FOLDER) -l$(ARA_TARGET_NAME) -L$(CPPUTEST_LIB_DIR) -lCppUTest -lm -lstdc++

# Simulation kernel and user interface libraries ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
INETMANET_LIB_DIR = inetmanet/src
OMNETPP_INCLUDE_PATH = $(INCLUDE_PATH) -Iinclude/omnetpp -Iinclude/inetmanet -I$(OMNETPP_INCL_DIR)
OMNETPP_LINKFLAGS = $(LINKFLAGS) $(OMNETPP_LIBS) -L$(INETMANET_LIB_DIR) -linet
OMNETPP_LINKFLAGS += -Wl,-rpath,$(INETMANET_LIB_DIR)
OMNETPP_LIB_PATH = $(OMNETPP_LIB_DIR)
OMNETPP_LD_LIB_PATH = LD_LIBRARY_PATH=$(OMNETPP_LIB_PATH)

# Build targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: all
all: libARA inetmanet_headers
	@echo -e "\n~~~ BUILDING OMNeT++ SIMULATIONS ~~~~\n"
	@cd omnetpp && $(MAKE)

.PHONY: libARA
libARA: $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME)

#
# Build the ARA library (libARA)
#
$(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME): $(LIBARA_O)
	@echo "Linking $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME)"
	@$(CXX) -shared -Wl,-soname,$(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) $(CFLAGS) $(INCLUDE_PATH) $(LINKFLAGS) $(LIBARA_O) \
		-o $(OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION)
	@cd $(LIBARA_SRC_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION) $(ARA_LIB_NAME)

# pull in dependeny file (if existing)
-include $(LIBARA_DEPENDENCIES)

#
# Builds all cpp files
#
$(OUTPUT_DIR)/%.o: %.cpp
	@$(MKPATH) $(dir $@)
	@echo "Compiling $*.cpp";
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) -c $*.cpp -o $@
	@$(CXX) $(CFLAGS) $(INCLUDE_PATH) $(ADDITIONAL_INCLUDES) -MM -MT $(OUTPUT_DIR)/$*.o $*.cpp > $(OUTPUT_DIR)/$*.d;

.PHONY: inetmanet_headers
inetmanet_headers: inetmanet/src/libinet.so
	@if [ ! -d include/inetmanet ]; then \
		echo -e "\n~~~ UPDATING INETMANET HEADERS ~~~"; \
		mkdir include/inetmanet; \
		for i in `find inetmanet/src/ -type f -name "*.h"`; do ln -s ../../$$i include/inetmanet/$${i##*/}; done; \
		echo "updating include/inetmanet/........done"; \
	fi

inetmanet/src/libinet.so: inetmanet/.git
	@echo -e "\n~~~ BUILDING INET/MANET FRAMEWORK ~~~\n"
	@cd inetmanet && $(MAKE) makefiles;
	@cd inetmanet && $(MAKE)

inetmanet/.git:
	@if [ ! -d inetmanet/src ]; then \
	echo -e "\n~~~ INITIALIZING INET/MANET SUBMODULE ~~~\n"; \
	git submodule init inetmanet; \
	git submodule update inetmanet; \
    fi

# Test targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: test
test: libAraTests

.PHONY: allTests
allTests: $(TESTS_FOLDER)/$(TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING complete TEST SUIT ~~~~~~"
	@if $(OMNETPP_LD_LIB_PATH) $(TESTS_FOLDER)/$(TEST_EXECUTABLE); then \
 		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
	else \
		echo -e "~~~ ERROR WHILE RUNNING TEST ~~~~~~~~\n"; \
		exit 1; \
	fi

.PHONY: libAraTests
libAraTests: $(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING libARA TEST SUIT ~~~~~~~~"	
	@if $(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE); then \
		echo -e "~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
	else \
		echo -e "~~~ ERROR WHILE RUNNING TEST ~~~~~~~~\n"; \
		exit 1; \
	fi

.PHONY: omnetTests
omnetTests: $(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	@echo -e "\n~~~ RUNNING OMNET ARA TEST SUIT ~~~~~"	
	@if $(OMNETPP_LD_LIB_PATH) $(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE); then \
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
	@echo "Building the comlete test suit ($(TESTS_FOLDER)/$(TEST_EXECUTABLE))"
	@$(CXX) $(TESTS_CFLAGS) $(OMNETARA_O) $(ALL_TEST_BINARIES) \
                $(INCLUDE_PATH) $(OMNETPP_INCLUDE_PATH) $(TESTS_LINKFLAGS) $(OMNETPP_LINKFLAGS) \
                -o $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(TEST_EXECUTABLE)
	@cd $(TESTS_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$(TESTS_FOLDER)/$(TEST_EXECUTABLE) $(TEST_EXECUTABLE)

#
# Tests only the libara sources
#
$(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) \
                                           $(CPPUTEST_LIB) \
                                           $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN)
	@echo "Building the libara test suit ($(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE))"
	@$(CXX) $(TESTS_CFLAGS) $(TESTAPI_BIN) $(TESTAPI_TESTS_BIN) $(LIBARA_TESTS_BIN) \
                $(INCLUDE_PATH) $(TESTS_LINKFLAGS) \
                -o $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE)
	@cd $(TESTS_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE) $(LIBARA_TEST_EXECUTABLE)

#
# Tests only the omnetpp ARA sources
#
$(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE): $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) \
                                                $(CPPUTEST_LIB) \
                                                $(TESTAPI_BIN) $(OMNETPP_ARA_TESTS_BIN)
	@echo "Building the omnetara test suit ($(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE))"
	@$(CXX) $(TESTS_CFLAGS) $(OMNETARA_O) $(TESTAPI_BIN) $(OMNETPP_ARA_TESTS_BIN) \
                $(INCLUDE_PATH) $(OMNETPP_INCLUDE_PATH) $(TESTS_LINKFLAGS) $(OMNETPP_LINKFLAGS) \
                -o $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE)
	@cd $(TESTS_FOLDER) && ln -s -f ../$(OUTPUT_DIR)/$(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE) $(OMNETPP_ARA_TEST_EXECUTABLE)

# Other targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#
# Builds the CppUTest Framework
#
$(CPPUTEST_LIB): $(CPPUTEST_BASE_DIR)/.git
	@echo -e "\n~~~ BUILDING CPPUTEST FRAMEWORK ~~~~~\n"
	@cd $(CPPUTEST_BASE_DIR) && $(MAKE)

$(CPPUTEST_BASE_DIR)/.git:
	@echo -e "\n~~~ INITIALIZING CPPUTEST SUBMODULE ~\n";
	@git submodule init tests/cpputest;
	@git submodule update tests/cpputest;

#
# Clean up (delete) all generated files
#
.PHONY: clean
clean:
	@echo "Cleaning libARA.."
	@rm -f $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) $(OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION)
	@rm -f $(LIBARA_O)
	@rm -f $(LIBARA_DEPENDENCIES)
	@echo "Cleaning omnetARA.."
	@cd omnetpp && $(MAKE) clean
	@echo "Cleaning tests.."
	@rm -f $(ALL_TEST_BINARIES)
	@rm -f $(TESTS_DEPENDENCIES)
	@rm -f $(TESTS_FOLDER)/$(TEST_EXECUTABLE) $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(TEST_EXECUTABLE)
	@rm -f $(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE) $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(LIBARA_TEST_EXECUTABLE)
	@rm -f $(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE) $(OUTPUT_DIR)/$(TESTS_FOLDER)/$(OMNETPP_ARA_TEST_EXECUTABLE)

.PHONY: cleanall
cleanall: clean
	@echo "Deleting submodule $($CPPUTEST_BASE_DIR)"
	@rm -R -f $(CPPUTEST_BASE_DIR)
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
     
