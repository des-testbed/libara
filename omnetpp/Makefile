#
# OMNeT++/OMNEST Makefile for ARA-Simulation
#
# This file was generated with the command:
#  opp_makemake -f --deep -I ../src/core -I ../src/exceptions -I ../src/util -I ../include -I ../include/inetmanet -L../inetmanet/src -linet -L../src -lara
#

# Name of target to be created (-o option)
TARGET = ARA-Simulation$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I../src/core \
    -I../src/exceptions \
    -I../src/util \
    -I../include \
    -I../include/inetmanet \
    -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L../inetmanet/src -L../src  -linet -lara
LIBS += -Wl,-rpath,`abspath ../inetmanet/src` -Wl,-rpath,`abspath ../src`

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = /omnetpp
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cpp and .msg files
OBJS = \
    $O/ARATrafficGenerator.o \
    $O/Debugger.o \
    $O/OMNeTAddress.o \
    $O/OMNeTARAClient.o \
    $O/OMNeTGate.o \
    $O/OMNeTPacket.o \
    $O/OMNeTStochasticForwardingPolicy.o

# Message files
MSGFILES =

#------------------------------------------------------------------------------

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

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# inserted from file 'makefrag':
COPTS += -std=c++11

# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cpp

$O/%.o: %.cpp $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cpp %_m.h: %.msg
	$(MSGC) -s _m.cpp $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f ARA-Simulation ARA-Simulation.exe libARA-Simulation.so libARA-Simulation.a libARA-Simulation.dll libARA-Simulation.dylib
	-rm -f ./*_m.cpp ./*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cpp

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/ARATrafficGenerator.o: ARATrafficGenerator.cpp \
  ../include/ARATrafficGenerator.h
$O/Debugger.o: Debugger.cpp \
  ../include/Debugger.h
$O/OMNeTAddress.o: OMNeTAddress.cpp \
  ../include/OMNeTAddress.h \
  ../include/Address.h
$O/OMNeTARAClient.o: OMNeTARAClient.cpp \
  ../include/OMNeTStochasticForwardingPolicy.h \
  ../include/NextHop.h \
  ../include/OMNeTAddress.h \
  ../include/AbstractARAClient.h \
  ../include/OMNeTGate.h \
  ../include/BestPheromoneForwardingPolicy.h \
  ../include/OMNeTARAClient.h \
  ../include/OMNeTPacket.h \
  ../include/ForwardingPolicy.h \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/PacketTrap.h \
  ../include/NetworkInterface.h \
  ../include/RoutingTable.h \
  ../include/StochasticForwardingPolicy.h \
  ../include/PacketType.h
$O/OMNeTGate.o: OMNeTGate.cpp \
  ../include/OMNeTAddress.h \
  ../include/OMNeTGate.h \
  ../include/OMNeTPacket.h \
  ../include/Address.h \
  ../include/Packet.h \
  ../include/NetworkInterface.h \
  ../include/PacketType.h
$O/OMNeTPacket.o: OMNeTPacket.cpp \
  ../include/OMNeTAddress.h \
  ../include/OMNeTPacket.h \
  ../include/Address.h \
  ../include/Packet.h \
  ../include/PacketType.h
$O/OMNeTStochasticForwardingPolicy.o: OMNeTStochasticForwardingPolicy.cpp \
  ../include/OMNeTStochasticForwardingPolicy.h \
  ../include/NextHop.h \
  ../include/ForwardingPolicy.h \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/RoutingTable.h \
  ../include/NetworkInterface.h \
  ../include/StochasticForwardingPolicy.h \
  ../include/PacketType.h
