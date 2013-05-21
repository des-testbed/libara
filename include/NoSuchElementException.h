/*
 * $FU-Copyright$
 */

#ifndef NOSUCHELEMENTEXCEPTION_H_
#define NOSUCHELEMENTEXCEPTION_H_

#include <string>
#include "Exception.h"

namespace ARA {

class NoSuchElementException: public Exception {
public:
    NoSuchElementException() {};
    NoSuchElementException(const char* message) : Exception(message) {};
};

} /* namespace ARA */
#endif /* NOSUCHELEMENTEXCEPTION_H_ */
