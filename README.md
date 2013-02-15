Ara-Sim
=======
An enhanceable framework for developing and testing the Ant Routing Algorithm (ARA) as proposed by M. Güneş, U. Sorges and I. Bouazizi in 2002.


Installation from git
---------------------
1. Clone the repository:

		git clone git://github.com/des-testbed/Ara-Sim.git
		
2. Please install the required dependencies

		g++ >= 4.7
		OMNeT++ >= 4.2

3. After the checkout you should run the following make target which will create src/Makefile and prepare the OMNeT++build.

		make makefiles

4. Now you should be able to build the sources and simulation via 

		make all

4. Ara-Sim is developed with a test driven approach. 
You should can run the whole Ara-Sim regression test suit via the following command.
This will automatically initialize and update the git submodule [tests/cpputest][2].

		make test


Hacking
-------

If you are new to the framework you should take a look at the [AbstractARAClientTest][3] which can give you a good feeling on how everything works.
If you are only interested in the OMNet++ part you should look at [omnetpp][4] and of course the [simulations][5] folder.


Bug tracker
-----------
Have a bug or feature request? Please create an issue here on GitHub!

https://github.com/des-testbed/ara-sim/issues

License
-------
tba.

[1]: http://www.cpputest.org/
[2]: https://github.com/FGrosse/cpputest
[3]: https://github.com/des-testbed/Ara-Sim/blob/develop/tests/core/AbstractARAClientTest.cpp
[4]: https://github.com/des-testbed/Ara-Sim/tree/develop/omnetpp
[5]: https://github.com/des-testbed/Ara-Sim/tree/develop/simulations
