/*
 * $FU-Copyright$
 */

#include "ThreadInterruptedException.h"

ARA_NAMESPACE_BEGIN

ThreadInterruptedException::ThreadInterruptedException() {

}

ThreadInterruptedException::ThreadInterruptedException(const char* message) : Exception(message) {

}

ARA_NAMESPACE_END
