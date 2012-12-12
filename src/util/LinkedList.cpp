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

#define LINKEDLIST_H_CPP_
#include "LinkedList.h"

#include "NoSuchElementException.h"

namespace ARA {

template<class T>
LinkedList<T>::~LinkedList() {
    // Delete all ListElements
    ListElement* currentElement;
    while(lastElement != NULL) {
        currentElement = lastElement;
        lastElement = lastElement->previous;
        delete currentElement;

    }
}

template<class T>
void LinkedList<T>::add(T* newElement) {
    ListElement* newListElement = new ListElement();
    newListElement->data = newElement;
    newListElement->next = NULL;

    if(firstElement == NULL) {
        newListElement->previous = NULL;
        firstElement = newListElement;
        lastElement = firstElement;
    }
    else {
        newListElement->previous = lastElement;
        lastElement->next = newListElement;
        lastElement = newListElement;
    }
}

template<class T>
T* LinkedList<T>::get(unsigned int index) {
    unsigned int counter = 0;
    ListElement* currentElement = firstElement;
    do {
        if(currentElement == NULL) {
            //TODO improve the exception message by supplying the attempted index
            throw Exception("Index out of bounds");
        }

        if(counter == index) {
            return currentElement->data;
        }
        else {
            currentElement = currentElement->next;
        }
        counter++;
    } while(true);
}

template<class T>
T* LinkedList<T>::remove() {
    if(firstElement == NULL) {
        throw NoSuchElementException("Can not remove the first element of an empty list.");
    }

    T* removedElement = firstElement->data;
    ListElement* removedListElement = firstElement;

    if(firstElement->next != NULL) {
        firstElement = firstElement->next;
        firstElement->previous = NULL;
    }
    else {
        // deleted the last remaining element in this list
        firstElement = NULL;
        lastElement = NULL;
    }

    delete removedListElement;
    return removedElement;
}

template<class T>
bool LinkedList<T>::isEmpty() {
    return lastElement == NULL;
}

} /* namespace ARA */
