/*
 * $FU-Copyright$
 */

#ifndef STACK_TRACE_H_
#define STACK_TRACE_H_

#include "ARAMacros.h"

#include <cxxabi.h>
#include <iostream>
#include <execinfo.h>

ARA_NAMESPACE_BEGIN

static inline void printStacktrace(unsigned int frames = 63) {
    /// the void pointers represent the current stack 
    void* tracePointers[frames];
    /// obtains the stack trace of the current thread
    int count = backtrace(tracePointers, sizeof(tracePointers)/sizeof(void*));

    if (count == 0) {
        std::cerr << "obtaining current stack trace failed. it's probably corrupt." << std::endl;
        return;
    }

    /// resolve the pointers to strings
    char** functionNames = backtrace_symbols(tracePointers, count);

    /// get a status flag for the demangle operation (see below)
    int status = -1;

    /// print the stack trace
    for (int i = 0; i < count; i++ ) {
        /// we need to demangle the function name first
        char *demangledName = abi::__cxa_demangle(functionNames[i], NULL, NULL, &status ); 

        if (status) { 
            std::cout << demangledName << std::endl; 
        }

        free(demangledName);
    }

    /// free the string pointers
    free(functionNames);
}

ARA_NAMESPACE_END

#endif
