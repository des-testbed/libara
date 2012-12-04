CXX = g++
INCLUDE_PATH= -I../include
CFLAGS = -g $(INCLUDE_PATH)
LINKFLAGS= -lcppunit

# Output directory
ifeq ("$(CONFIGNAME)","")
CONFIGNAME = 'gcc-debug'
endif
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

TEST_SRC = ./*.cpp
COMPILED_SRC = $(shell find $(OUTPUT_DIR) -type f -name '*.o')

all: $(TEST_SRC)	
	$(CXX) $(CFLAGS) -o runAllTests TestRunner.cpp $(COMPILED_SRC) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)

.cpp.o: checkcppunit
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o TestResults.xml runAllTests
	
checkcppunit:
	@if ! command -v cppunit-config >/dev/null ; then \
	echo >&2; \
	echo '======================================================================='>&2; \
	echo 'ERROR: It seems like CppUnit is not installed on this system.'>&2; \
	echo '======================================================================='>&2; \
	echo>&2; \
	exit 1; \
	fi