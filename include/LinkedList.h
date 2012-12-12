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

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stddef.h>

namespace ARA {

template <class T>
class LinkedList {

    struct ListElement {
        T* data;
        ListElement* previous;
        ListElement* next;
    };

public:
    ~LinkedList();

    /**
     * Appends the specified element to the end of this list.
     */
    void add(T* newElement);

    /**
     * Returns the element at the specified position in this list.
     * Note that the index is always zero based. The first valid index is zero (0).
     */
    T* get(unsigned int index);

    /**
     * Retrieves and removes the head (first element) of this list.
     */
    T* remove();

    /**
     * Returns true if this list contains no elements.
     */
    bool isEmpty();

    /**
     * Returns the number of elements in this list.
     */
    unsigned int size();

    /**
     * Returns true if this list contains the specified element.
     * To accomplish this each element of this list is compared to the
     * searched element by using the operator==.
     *
     * This means that the class T *has to implement* operator==
     */
    bool contains(T* searchedElement);

private:
    ListElement* firstElement = NULL;
    ListElement* lastElement = NULL;

    unsigned int numberOfElements = 0;
};

} /* namespace ARA */

/**
 * Declaration and implementation of a template class have to be in the same file so
 * the corresponding cpp file is included here if it has not been included earlier.
 */
#ifndef LINKEDLIST_H_CPP_
#include "../src/util/LinkedList.cpp"
#endif /* LINKEDLIST_H_CPP_ */

#endif /* LINKEDLIST_H_ */
