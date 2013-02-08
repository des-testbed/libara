#
# OMNeT++/OMNEST Makefile for libara
#
# This file was generated with the command:
#  opp_makemake -f --deep --make-so -o ara -I ../include
#

# Name of target to be created (-o option)
TARGET = libara$(SHARED_LIB_SUFFIX)

# C++ include paths (with -I)
INCLUDE_PATH = -I../include -I. -Icore -Iexceptions -Iutil

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = /src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cpp and .msg files
OBJS = \
    $O/core/AbstractARAClient.o \
    $O/core/BestPheromoneForwardingPolicy.o \
    $O/core/CubicEvaporationPolicy.o \
    $O/core/EvaporationPolicy.o \
    $O/core/LinearEvaporationPolicy.o \
    $O/core/NextHop.o \
    $O/core/Packet.o \
    $O/core/PacketTrap.o \
    $O/core/RoutingTable.o \
    $O/core/RoutingTableEntry.o \
    $O/core/StochasticForwardingPolicy.o \
    $O/exceptions/Exception.o \
    $O/exceptions/NoSuchElementException.o \
    $O/util/Pair.o \
    $O/util/Time.o

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
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)

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
	$(SHLIB_LD) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS)
	$(SHLIB_POSTPROCESS) $O/$(TARGET)

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
	-rm -f ara ara.exe libara.so libara.a libara.dll libara.dylib
	-rm -f ./*_m.cpp ./*_m.h
	-rm -f core/*_m.cpp core/*_m.h
	-rm -f exceptions/*_m.cpp exceptions/*_m.h
	-rm -f util/*_m.cpp util/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cpp core/*.cpp exceptions/*.cpp util/*.cpp

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/core/AbstractARAClient.o: core/AbstractARAClient.cpp \
  ../include/ForwardingPolicy.h \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/NextHop.h \
  ../include/PacketTrap.h \
  ../include/LinearEvaporationPolicy.h \
  ../include/RoutingTable.h \
  ../include/NetworkInterface.h \
  ../include/EvaporationPolicy.h \
  ../include/AbstractARAClient.h \
  ../include/PacketType.h \
  ../include/Time.h
$O/core/BestPheromoneForwardingPolicy.o: core/BestPheromoneForwardingPolicy.cpp \
  ../include/ForwardingPolicy.h \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/NextHop.h \
  ../include/RoutingTable.h \
  ../include/NetworkInterface.h \
  ../include/LinearEvaporationPolicy.h \
  ../include/EvaporationPolicy.h \
  ../include/PacketType.h \
  ../include/BestPheromoneForwardingPolicy.h \
  ../include/Time.h
$O/core/CubicEvaporationPolicy.o: core/CubicEvaporationPolicy.cpp \
  ../include/CubicEvaporationPolicy.h \
  ../include/EvaporationPolicy.h \
  ../include/Time.h
$O/core/EvaporationPolicy.o: core/EvaporationPolicy.cpp \
  ../include/EvaporationPolicy.h \
  ../include/Time.h
$O/core/LinearEvaporationPolicy.o: core/LinearEvaporationPolicy.cpp \
  ../include/LinearEvaporationPolicy.h \
  ../include/EvaporationPolicy.h \
  ../include/Time.h
$O/core/NextHop.o: core/NextHop.cpp \
  ../include/Address.h \
  ../include/Packet.h \
  ../include/NextHop.h \
  ../include/NetworkInterface.h \
  ../include/PacketType.h
$O/core/Packet.o: core/Packet.cpp \
  ../include/Address.h \
  ../include/Packet.h \
  ../include/PacketType.h
$O/core/PacketTrap.o: core/PacketTrap.cpp \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/NextHop.h \
  ../include/PacketTrap.h \
  ../include/LinearEvaporationPolicy.h \
  ../include/NetworkInterface.h \
  ../include/RoutingTable.h \
  ../include/EvaporationPolicy.h \
  ../include/PacketType.h \
  ../include/Time.h
$O/core/RoutingTable.o: core/RoutingTable.cpp \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/NextHop.h \
  ../include/LinearEvaporationPolicy.h \
  ../include/NetworkInterface.h \
  ../include/RoutingTable.h \
  ../include/EvaporationPolicy.h \
  ../include/PacketType.h \
  ../include/Time.h
$O/core/RoutingTableEntry.o: core/RoutingTableEntry.cpp \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/NextHop.h \
  ../include/NetworkInterface.h \
  ../include/PacketType.h
$O/core/StochasticForwardingPolicy.o: core/StochasticForwardingPolicy.cpp \
  ../include/ForwardingPolicy.h \
  ../include/Address.h \
  ../include/RoutingTableEntry.h \
  ../include/Packet.h \
  ../include/NextHop.h \
  ../include/RoutingTable.h \
  ../include/NetworkInterface.h \
  ../include/LinearEvaporationPolicy.h \
  ../include/EvaporationPolicy.h \
  ../include/PacketType.h \
  ../include/StochasticForwardingPolicy.h \
  ../include/Time.h
$O/exceptions/Exception.o: exceptions/Exception.cpp \
  ../include/Exception.h
$O/exceptions/NoSuchElementException.o: exceptions/NoSuchElementException.cpp \
  ../include/Exception.h \
  ../include/NoSuchElementException.h
$O/util/Pair.o: util/Pair.cpp \
  ../include/../src/util/Pair.cpp \
  ../include/Pair.h
$O/util/Time.o: util/Time.cpp \
  ../include/Time.h
