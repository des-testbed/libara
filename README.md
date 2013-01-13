Ara-Sim
=======

An enhanceable framework for developing and testing the Ant Routing Algorithm (ARA) as proposed by M. Güneş, U. Sorges and I. Bouazizi in 2002.


Installation
============

After gut git checkout you may run first
```
make makefiles
```

This will automagically create the Makefile for the src folder and prepare the OMNeT++ build.

After that you should install the [CppUTest Framework][1].
There is also a Makefile target to do that.

```
make installCppUTest
```

This will clone [a fork of CppUTest][2] into a subfolder of your Ara-Sim working copy, build it and run some tests on it.
After you have completetd this step you can run the whole Ara-Sim regression test suit via

```
make test
```


Where to go next?
=================

If you are new to the framework you should take a look at the [AbstractARAClientTest][3] which can give you a good feeling on how everything works.
If you are only interested in the OMNet++ part you should look at [src/omnetpp][4] and of course the [simulations][5] folder.


[1]: http://www.cpputest.org/
[2]: https://github.com/FGrosse/cpputest
[3]: https://github.com/FGrosse/Ara-Sim/blob/develop/tests/core/AbstractARAClientTest.cpp
[4]: https://github.com/FGrosse/Ara-Sim/tree/develop/src/omnetpp
[5]: https://github.com/FGrosse/Ara-Sim/tree/develop/simulations
