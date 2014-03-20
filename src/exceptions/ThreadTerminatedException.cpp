/*
 * $FU-Copyright$
 */

#include "ThreadTerminatedException.h"

ARA_NAMESPACE_BEGIN

ThreadTerminatedException::ThreadTerminatedException() {

}

ThreadTerminatedException::ThreadTerminatedException(const char* message) : Exception(message) {

}

ARA_NAMESPACE_END
