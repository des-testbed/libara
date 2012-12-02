CXX = g++
INCLUDES= -I../include
CFLAGS = -g $(INCLUDES)
LINKFLAGS= -lcppunit
COMPILED_SRC = $(shell find ../out/$(CONFIGNAME)/src/ -type f -name '*.o')
TEST_SRC = ./*.cpp
INCLUDE_PATH = -I../include

all: $(TEST_SRC)
	$(CXX) $(CFLAGS) -o runAllTests TestRunner.cpp $(COMPILED_SRC) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o TestResults.xml runAllTests