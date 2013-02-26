Ara-Sim
=======
An enhanceable framework for developing and testing the Ant Routing Algorithm (ARA) as proposed by M. Güneş, U. Sorges and I. Bouazizi in 2002.


Installation from git
---------------------
1. Clone the repository:

		git clone git://github.com/des-testbed/Ara-Sim.git
		
2. Please install the required dependencies

		clang >= 3.2
		OMNeT++ >= 4.2

3. After the checkout you can build the sources and simulation via 

		make all

4. Ara-Sim is developed with a test driven approach using the [CppUTest][3] framework. 
You should can run the whole Ara-Sim regression test suit via the following command.
This will automatically initialize and update the git submodule [tests/cpputest][4].

		make test


Hacking
-------

If you are new to the framework you should take a look at the [AbstractARAClientTest][5] which can give you a good feeling on how everything works.
If you are only interested in the OMNet++ part you should look at [omnetpp][6] and of course the [simulations][7] folder.


Bug tracker
-----------
Have a bug or feature request? Please create an issue here on GitHub!

https://github.com/des-testbed/ara-sim/issues

License
-------
tba.

[1]: http://clang.llvm.org/
[2]: http://www.omnetpp.org/component/docman/cat_view/17-downloads/1-omnet-releases
[3]: http://www.cpputest.org/
[4]: https://github.com/FGrosse/cpputest
[5]: https://github.com/des-testbed/Ara-Sim/blob/develop/tests/core/AbstractARAClientTest.cpp
[6]: https://github.com/des-testbed/Ara-Sim/tree/develop/omnetpp
[7]: https://github.com/des-testbed/Ara-Sim/tree/develop/simulations
