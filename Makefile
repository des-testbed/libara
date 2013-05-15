include Makefile.inc

# Source folder ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LIBARA_SRC_FOLDER = src

# Output directory and files ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PROJECT_OUTPUT_DIR = out/$(CONFIGNAME)
LIBARA_SRC = $(shell find $(LIBARA_SRC_FOLDER)/ -type f -name '*.cpp')
LIBARA_O = $(subst .cpp,.o, $(addprefix $(PROJECT_OUTPUT_DIR)/, $(LIBARA_SRC)))
LIBARA_DEPENDENCIES = $(LIBARA_O:.o=.d)

# Compiler options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CFLAGS += -std=c++11 -fPIC
INCLUDE_PATH = -Iinclude
COPTS = $(CFLAGS) $(INCLUDE_PATH)

# Build targets ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
all: libARA inetmanet_headers
	@echo -e "\n~~~ BUILDING OMNeT++ SIMULATIONS ~~~~\n"
	@cd omnetpp && $(MAKE)

libARA: $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME)

#
# Build the ARA library (libARA)
#
$(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME): $(LIBARA_O)
	@echo "Linking $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME)"
	@$(CXX) -shared -Wl,-soname,$(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) $(COPTS) $(LINKFLAGS) $(LIBARA_O) \
		-o $(PROJECT_OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION)
	@cd $(LIBARA_SRC_FOLDER) && ln -s -f ../$(PROJECT_OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION) $(ARA_LIB_NAME)

# pull in dependeny file (if existing) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-include $(LIBARA_DEPENDENCIES)

#
# Builds all cpp files
#
$(PROJECT_OUTPUT_DIR)/%.o: %.cpp
	@$(MKPATH) $(dir $@)
	@echo "Compiling $*.cpp";
	@$(CXX) $(COPTS) -c $*.cpp -o $@
	@$(CXX) $(COPTS) -MM -MT $(PROJECT_OUTPUT_DIR)/$*.o $*.cpp > $(PROJECT_OUTPUT_DIR)/$*.d;

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

test: libARA
	@echo -e "\n~~~ BUILDING libARA TESTS ~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runLibAraTests NO_OMNET=TRUE
	
omnetTests: all 
	@echo -e "\n~~~ BUILDING omnetARA TESTS ~~~~~~~~~\n"
	@cd tests && $(MAKE) runOmnetAraTests 

allTests: all
	@echo -e "\n~~~ BUILDING all TESTS ~~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) all

runSingleTest: all
	@echo -e "\n~~~ RUNNING SINGLE TEST ~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runSingleTest

clean:
	rm -f $(LIBARA_SRC_FOLDER)/$(ARA_LIB_NAME) $(PROJECT_OUTPUT_DIR)/$(ARA_SONAME).$(ARA_MINOR_VERSION)
	rm -f $(LIBARA_O)
	rm -f $(LIBARA_DEPENDENCIES)
	@cd omnetpp && $(MAKE) clean
	@cd tests && $(MAKE) clean
	rm -R -f out/$(CONFIGNAME)

release: allTests
	@echo -e "\n~~~ PUSHING TO GIT ORIGIN ~~~~~~~~~~~\n"
	git push
		
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
     
