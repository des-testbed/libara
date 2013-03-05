# Make "debug" the default mode
ifndef MODE
MODE = debug
endif

# TODO we might want to make this more generic (i.e. enable compiling with gcc)
# A simple ifndef does not work because this will always be set by the OMNeT+ makefile.inc to gcc
CXX = clang

# Configname determines where (in which subdirectory of out/)
# makemake-generated makefiles create object files and other
# build artifacts.
CONFIGNAME = $(CXX)-$(MODE)

ARA_MAJOR_VERSION = 0
ARA_MINOR_VERSION = 5
ARA_TARGET_NAME = ARA
ARA_LIB_NAME = lib$(ARA_TARGET_NAME).so
ARA_SONAME = $(ARA_LIB_NAME).$(ARA_MAJOR_VERSION)

#
# Various tools and options
#
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
