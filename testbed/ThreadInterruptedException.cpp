#include "ThreadInterruptedException.h"

TESTBED_NAMESPACE_BEGIN

ThreadInterruptedException::ThreadInterruptedException(){ }

ThreadInterruptedException::ThreadInterruptedException(const char* message):Exception(message){ }

TESTBED_NAMESPACE_END
