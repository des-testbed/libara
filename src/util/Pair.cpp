/*
 * $FU-Copyright$
 */

#define PAIR_H_CPP_
#include "Pair.h"

namespace ARA {

template<class Left, class Right>
Pair<Left, Right>::Pair(Left left, Right right) {
    this->left = left;
    this->right = right;
}

template<class Left, class Right>
Left Pair<Left, Right>::getLeft() {
    return left;
}

template<class Left, class Right>
Right Pair<Left, Right>::getRight() {
    return right;
}

} /* namespace ARA */
