/*
 * $FU-Copyright$
 */

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
