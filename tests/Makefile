CXX = g++
INCLUDES= -I./
CXXFLAGS = -g $(INCLUDES)
LINKFLAGS= -lcppunit
COMPILED_SRC = ../out/$(CONFIGNAME)/src/*.o
TEST_SRC = ./*.cpp

all: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -o runAllTests TestRunner.cpp $(COMPILED_SRC) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o TestResults.xml runAllTests