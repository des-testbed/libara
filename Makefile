include Makefile.inc

all: libARA inetmanet_headers
	@echo -e "\n~~~ BUILDING OMNeT++ SIMULATIONS ~~~~\n"
	@cd omnetpp && $(MAKE)

libARA: 
	@echo -e "\n~~~ BUILDING libARA ~~~~~~~~~~~~~~~~~\n"
	@cd src && $(MAKE)

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
	@echo -e "\n~~~ BUILDING ALL TESTS ~~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runTests	
	
omnetTest: all 
	@echo -e "\n~~~ BUILDING libARA TESTS ~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runLibAraTests NO_OMNET=TRUE

runSingleTest: all
	@echo -e "\n~~~ RUNNING SINGLE TEST ~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runSingleTest

clean:
	@cd src && $(MAKE) clean
	@cd omnetpp && $(MAKE) clean
	@cd tests && $(MAKE) clean
	rm -R -f out/$(CONFIGNAME)

release: clean test	
		
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
     
