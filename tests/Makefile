CXX = g++
INCLUDE_PATH= -I../include
CFLAGS = -g $(INCLUDE_PATH)
LINKFLAGS= -lUnitTest++

# Output directory
ifeq ("$(CONFIGNAME)","")
CONFIGNAME = 'gcc-debug'
endif
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

TEST_SRC = ./*.cpp
COMPILED_SRC = $(shell find $(OUTPUT_DIR) -type f -name '*.o')

all: check_if_unittest_is_installed $(TEST_SRC)	
	$(CXX) $(CFLAGS) -o runAllTests TestRunner.cpp $(COMPILED_SRC) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)

.cpp.o: 
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o TestResults.xml runAllTests
	
check_if_unittest_is_installed:
	@if ! [ -e "/usr/include/libUnitTest++.a" ]; then \
	echo >&2; \
	echo '======================================================================='>&2; \
	echo 'ERROR: It seems like UnitTest++ library is not installed on this system'>&2; \
	echo '======================================================================='>&2; \
	echo>&2; \
	exit 1; \
	fi