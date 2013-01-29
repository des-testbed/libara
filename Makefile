all: checkmakefiles inetmanet
	@echo -e "\n~~~ BUILDING SOURCE ~~~~~~~~~~~~~~~~~\n"
	@cd src && $(MAKE)

inetmanet: inetmanet_headers	

inetmanet_headers: inetmanet/src/libinet.so
	@echo "Updating INET/MANET header symlinks in include/inetmanet";
	@if [ ! -d include/inetmanet ]; then \
	mkdir include/inetmanet; \
	fi
	@rm -f include/inetmanet/*	
	@for i in `find inetmanet/src/ -type f -name "*.h"`; do ln -s ../../$$i include/inetmanet/$${i##*/}; done

inetmanet/src/libinet.so:
	@echo -e "\n~~~ BUILDING INET/MANET FRAMEWORK ~~~\n"
	@cd inetmanet && $(MAKE) makefiles;
	@cd inetmanet && $(MAKE)

test: all
	@echo -e "\n~~~ BUILDING TESTS ~~~~~~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runTests	

runSingleTest: all
	@echo -e "\n~~~ RUNNING SINGLE TEST ~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runSingleTest

clean: checkmakefiles
	@cd src && $(MAKE) clean
	@cd tests && $(MAKE) clean

cleanall: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@cd tests && $(MAKE) clean
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
     
