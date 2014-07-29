/*
 * $FU-Copyright$
 */

#include "Exception.h"

namespace ARA {

Exception::Exception(const char* message) {
    this->message = message;
}

const char* Exception::getMessage() {
    return message;
}

} /* namespace ARA */
