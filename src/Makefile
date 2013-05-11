include ../Makefile.inc

# Output directory and files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = ../out/$(CONFIGNAME)
LIBARA_OUTPUT_DIR = $(PROJECT_OUTPUT_DIR)/libara
LIBARA_SRC = $(shell find ./ -type f -name '*.cpp')
LIBARA_O = $(subst .cpp,.o, $(addprefix $(LIBARA_OUTPUT_DIR)/, $(LIBARA_SRC)))

# Compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS += -std=c++11 -fPIC
INCLUDE_PATH = -I../include -I/usr/include/c++/4.8.0/
COPTS = $(CFLAGS) $(INCLUDE_PATH)

# Build targets~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
all: $(ARA_LIB_NAME)

#
# Build the ARA library
#
$(ARA_LIB_NAME): $(LIBARA_O)
	@echo "Building libara (./$(ARA_LIB_NAME))"
	@$(CXX) -shared -Wl,-soname,$(ARA_LIB_NAME) $(COPTS) $(LINKFLAGS) $(LIBARA_O) \
		-o $(LIBARA_OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION)
	@ln -s -f $(LIBARA_OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION) $(ARA_LIB_NAME)
	
#
# Builds all cpp files
#
$(LIBARA_OUTPUT_DIR)/%.o: %.cpp
	ls /usr/include/c++/4.8.0/
	ls /usr/include/c++/4.8.0/bits/
	ls /usr/include/c++/4.8/bits/ 
	@$(MKPATH) $(dir $@)
	@echo "Compiling $*.cpp";
	@$(CXX) $(COPTS) -c $*.cpp -o $@
	
#
# Delets all compiled objects
#
clean:
	rm -f $(ARA_LIB_NAME) $(LIBARA_OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION)
	rm -f $(LIBARA_O)
