#include "TimeFactoryMock.h"
#include "testAPI/mocks/TimeMock.h"

using namespace ARA;

Time* TimeFactoryMock::makeTime() {
    return new TimeMock();
}
