all: checkmakefiles
	@echo -e "\n~~~ BUILDING SOURCE ~~~~~~~~~~~~~~~~~\n"
	@cd src && $(MAKE)

test: all
	@echo -e "\n~~~ BUILDING TESTS ~~~~~~~~~~~~~~~~~~\n"
	@cd tests && $(MAKE) runTests	

clean: checkmakefiles
	@cd src && $(MAKE) clean
	@cd tests && $(MAKE) clean

cleanall: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@cd tests && $(MAKE) clean
	rm -f src/Makefile

makefiles:
	cd src && opp_makemake -f --deep -I ../include

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
		