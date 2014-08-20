/*
 * $FU-Copyright$
 */

#ifndef CONTEX_H_
#define CONTEX_H_

#include "ARAMacros.h"
#include "Packet.h"

ARA_NAMESPACE_BEGIN

/**
 * A common base class for the context objects of the various timers.
 */
class Context {
    public:
        Context(){};
        virtual ~Context(){};

        void setType(std::string typeName);
        virtual std::string toString();

    private:
        std::string type;
};

ARA_NAMESPACE_END

#endif // CONTEXT_H_
