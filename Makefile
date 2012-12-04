all: checkmakefiles
	cd src && $(MAKE)

test: all
	cd tests && $(MAKE)
	@echo -e "\n~~~ RUNNING COMPLETE TEST SUIT ~~~~~~\n"
	@if ./tests/runAllTests; then \
     echo -e "\n~~~ TESTS PASSED SUCCESSFULLY ~~~~~~~\n"; \
    fi
	
testclean:
	cd tests && $(MAKE) clean	

clean: checkmakefiles
	cd src && $(MAKE) clean
	cd tests && $(MAKE) clean

cleanall:
	checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	cd tests && $(MAKE) clean
	rm -f src/Makefile

makefiles:
	cd src && opp_makemake -f --deep

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi
