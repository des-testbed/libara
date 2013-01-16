/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#ifndef PAIR_H_
#define PAIR_H_

namespace ARA {

template<class Left, class Right>
class Pair {
public:
    Pair(Left left, Right right);

    Left getLeft();
    Right getRight();

private:
    Left left;
    Right right;
};

} /* namespace ARA */

/**
 * Declaration and implementation of a template class have to be in the same file so
 * the corresponding cpp file is included here if it has not been included earlier.
 */
#ifndef PAIR_H_CPP_
#include "../src/util/Pair.cpp"
#endif /* PAIR_H_CPP_ */

#endif /* PAIR_H_ */
