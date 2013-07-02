/*
 * $FU-Copyright$
 */

#include "TimeoutEventListenerMock.h"

using namespace ARA;

void TimeoutEventListenerMock::timerHasExpired(Timer* responsibleTimer, void* contextObject) {
    hasListenerBeenNotified = true;
}

bool TimeoutEventListenerMock::hasBeenNotified() {
    return hasListenerBeenNotified;
}
