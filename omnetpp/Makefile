# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)
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

# our makefile needs to be included after the omnet stuff (otherwise $(CXX) is overridden
include ../Makefile.inc
CONFIGNAME = $(CXX)-$(MODE)

# Output directory and files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = ../out/$(CONFIGNAME)
OMNET_ARA_OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/omnetpp
OMNET_ARA_SRC = $(shell find ./ -type f -name '*.cpp')
OMNET_ARA_O = $(subst .cpp,.o, $(addprefix $(OMNET_ARA_OUTPUT_DIR)/, $(OMNET_ARA_SRC)))
OMNET_ARA_MAIN_TARGET = ara-sim

# libARA stuff ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ARA_LIB_DIR = ../src
ARA_LIB = $(ARA_LIB_DIR)/$(ARA_LIB_NAME)

# Simulation kernel and user interface libraries ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
INETMANET_LIB_DIR = ../inetmanet/src
USERIF_LIBS = $(ALL_ENV_LIBS)

# Include path and compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS += -std=c++11
INCLUDE_PATH = \
    -I../include \
    -I../include/inetmanet \
    -I../include/omnetpp \
    -I.
LINKFLAGS = $(OMNETPP_LIBS) -L$(ARA_LIB_DIR) -l$(ARA_TARGET_NAME) -L$(INETMANET_LIB_DIR) -linet -lm
LINKFLAGS += -Wl,-rpath,$(INETMANET_LIB_DIR) -Wl,-rpath,$(ARA_LIB_DIR)
COPTS = $(CFLAGS) $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)


#
# Build everything into an exectutable target
#
all: $(OMNET_ARA_MAIN_TARGET)

#
# Build the omnetpp ARA exectutable
#
$(OMNET_ARA_MAIN_TARGET): $(OMNET_ARA_O)
	@echo "Building omnetpp ARA (./$(OMNET_ARA_MAIN_TARGET))"
	@$(CXX) $(LINKFLAGS) -o $(OMNET_ARA_OUTPUT_DIR)/$(OMNET_ARA_MAIN_TARGET) $(OMNET_ARA_O)
	@ln -s -f $(OMNET_ARA_OUTPUT_DIR)/$(OMNET_ARA_MAIN_TARGET) $(OMNET_ARA_MAIN_TARGET)
	
#
# Builds all cpp files
#
$(OMNET_ARA_OUTPUT_DIR)/%.o: %.cpp
	@$(MKPATH) $(dir $@)
	@echo "Compiling $*.cpp";
	@$(CXX) $(COPTS) -c $*.cpp -o $@
	
#
# Delets all compiled objects
#
clean:
	rm -f $(OMNET_ARA_MAIN_TARGET) $(OMNET_ARA_OUTPUT_DIR)/$(OMNET_ARA_MAIN_TARGET)
	rm -f $(OMNET_ARA_O)
