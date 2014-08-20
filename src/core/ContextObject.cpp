/*
 * $FU-Copyright$
 */

#include "ContextObject.h"

ARA_NAMESPACE_BEGIN

void ContextObject::setType(std::string typeInfo){
    type = typeInfo;
}

std::string ContextObject::toString(){
    return type;
}

ARA_NAMESPACE_END
