/*
 * $FU-Copyright$
 */

#ifndef THREAD_TERMINATED_EXCEPTION_H_
#define THREAD_TERMINATED_EXCEPTION_H_

#include "ARAMacros.h"
#include "Exception.h"

ARA_NAMESPACE_BEGIN

class ThreadTerminatedException : public Exception  {
    public:
        ThreadTerminatedException();
        ThreadTerminatedException(const char* message);
};

ARA_NAMESPACE_END

#endif 
