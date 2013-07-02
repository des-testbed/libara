/*
 * $FU-Copyright$
 */

#ifndef THREAD_INTERRUPTED_EXCEPTION_H_
#define THREAD_INTERRUPTED_EXCEPTION_H_

#include "Testbed.h"
#include "Exception.h"

TESTBED_NAMESPACE_BEGIN

class ThreadInterruptedException : public Exception  {
    public:
        ThreadInterruptedException();
        ThreadInterruptedException(const char* message);
};

TESTBED_NAMESPACE_END

#endif 
