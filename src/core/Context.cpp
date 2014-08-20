/*
 * $FU-Copyright$
 */

#include "Context.h"

ARA_NAMESPACE_BEGIN

void Context::setType(std::string typeInfo){
    type = typeInfo;
}

std::string Context::toString(){
    return type;
}

ARA_NAMESPACE_END
