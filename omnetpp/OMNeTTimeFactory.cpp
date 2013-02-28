#include "OMNeTTimeFactory.h"
#include "OMNeTTime.h"

using namespace ARA;
using namespace ARA::omnetpp;

Time* OMNeTTimeFactory::makeTime(){
    return new OMNeTTime();
}
