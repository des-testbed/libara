/*
 * $FU-Copyright$
 */

#ifndef CONTEX_OBJECT_H__
#define CONTEX_OBJECT_H__

#include "ARAMacros.h"
#include "Packet.h"

ARA_NAMESPACE_BEGIN

/**
 * A common base class for the context objects of the various timers.
 */
class ContextObject {
    public:
        ContextObject(){};
        virtual ~ContextObject(){};

        void setType(std::string typeName);
        virtual std::string toString();

    private:
        std::string type;
};

ARA_NAMESPACE_END

#endif // CONTEXT_OBJECT_H_
