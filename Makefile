all: checkmakefiles inetmanet_headers
	@echo -e "\n~~~ BUILDING SOURCE ~~~~~~~~~~~~~~~~~\n"
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

test: all
	@echo -e "\n~~~ BUILDING TESTS ~~~~~~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runTests	

runSingleTest: all
	@echo -e "\n~~~ RUNNING SINGLE TEST ~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runSingleTest

clean: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@cd tests && $(MAKE) clean
	rm -R -f include/inetmanet

cleanall: clean	
	rm -f src/Makefile

release: cleanall makefiles test	

makefiles: 
	cd src && opp_makemake -f --deep -I ../include -I ../include/inetmanet -L"../inetmanet/src" -linet

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi

installCppUTest:
	@cd tests && $(MAKE) installCppUTest
		
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
     
