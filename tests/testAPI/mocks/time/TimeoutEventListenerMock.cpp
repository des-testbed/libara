#include "TimeoutEventListenerMock.h"

using namespace ARA;

void TimeoutEventListenerMock::timerHasExpired(Timer* responsibleTimer) {
    hasListenerBeenNotified = true;
}

bool TimeoutEventListenerMock::hasBeenNotified() {
    return hasListenerBeenNotified;
}
