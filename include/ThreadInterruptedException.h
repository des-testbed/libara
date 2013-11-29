/*
 * $FU-Copyright$
 */

#ifndef THREAD_INTERRUPTED_EXCEPTION_H_
#define THREAD_INTERRUPTED_EXCEPTION_H_

#include "ARAMacros.h"
#include "Exception.h"

ARA_NAMESPACE_BEGIN

class ThreadInterruptedException : public Exception  {
    public:
        ThreadInterruptedException();
        ThreadInterruptedException(const char* message);
};

ARA_NAMESPACE_END

#endif 
